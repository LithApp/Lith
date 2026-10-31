#!/usr/bin/env python3
"""Runs clang-tidy and clazy on the C++ code changed since a base commit.

Only diagnostics on lines added or modified since the base commit are reported, so
warnings already present in the code base don't hide the ones a change introduces.
Requires a compilation database (CMAKE_EXPORT_COMPILE_COMMANDS) of a build in which the
generated sources already exist. In GitHub Actions, findings are emitted as annotations.

Example: dist/ci/static-analysis.py --build-dir build --base origin/master
"""

import argparse
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from pathlib import Path

SOURCE_SUFFIXES = (".cpp", ".cc", ".cxx", ".mm")
HEADER_SUFFIXES = (".h", ".hpp")
DIAGNOSTIC = re.compile(
    r"^(?P<file>[^\n:]+):(?P<line>\d+):(?P<col>\d+): (?P<level>warning|error): (?P<message>.*?)(?: \[(?P<check>[^\]\s]+)\])?$"
)


@dataclass(frozen=True)
class Finding:
    tool: str
    file: str
    line: int
    col: int
    check: str
    message: str


def git(*args: str) -> str:
    return subprocess.run(["git", *args], check=True, capture_output=True, text=True).stdout


def changed_lines(base: str) -> dict[str, set[int]]:
    """Lines added or modified in the working tree since base, keyed by repository-relative path."""
    pathspecs = [f"*{suffix}" for suffix in SOURCE_SUFFIXES + HEADER_SUFFIXES]
    diff = git("diff", "--unified=0", "--no-color", "--no-ext-diff", "--diff-filter=AM", base, "--", *pathspecs)
    result: dict[str, set[int]] = {}
    current = None
    for line in diff.splitlines():
        if line.startswith("+++ "):
            current = line[len("+++ b/") :] if line.startswith("+++ b/") else None
        elif line.startswith("@@") and current:
            match = re.match(r"@@ -\S+ \+(\d+)(?:,(\d+))? @@", line)
            start, count = int(match[1]), int(match[2] if match[2] is not None else 1)
            if count:
                result.setdefault(current, set()).update(range(start, start + count))
    return result


def translation_units(build_dir: Path, root: Path, changed: dict[str, set[int]]) -> list[Path]:
    """Sources to analyze: the changed ones, and for changed headers the sources including them."""
    database = json.loads((build_dir / "compile_commands.json").read_text())
    sources = set()
    for entry in database:
        path = Path(entry["directory"], entry["file"]).resolve()
        if path.is_relative_to(root) and not path.is_relative_to(build_dir) and "_deps" not in path.parts:
            sources.add(path)

    units = set()
    for relative in changed:
        path = root / relative
        if path.suffix in SOURCE_SUFFIXES:
            if path in sources:
                units.add(path)
            continue
        siblings = {source for source in sources if source.parent == path.parent and source.stem == path.stem}
        if siblings:
            units |= siblings
            continue
        include = re.compile(rf'#\s*include\s*["<](?:[^">]*/)?{re.escape(path.name)}[">]')
        units |= {source for source in sources if include.search(source.read_text(errors="replace"))}
    return sorted(units)


def run_tool(tool: str, command: list[str], root: Path) -> tuple[set[Finding], str]:
    process = subprocess.run(command, capture_output=True, text=True, cwd=root)
    findings = set()
    for line in (process.stdout + process.stderr).splitlines():
        match = DIAGNOSTIC.match(line)
        if not match:
            continue
        check = match["check"] or ""
        # Plain compiler diagnostics aren't the job of either tool here
        if tool == "clazy" and not check.startswith("-Wclazy-"):
            continue
        if tool == "clang-tidy" and (not check or check.startswith("clang-diagnostic-")):
            continue
        path = Path(root, match["file"]).resolve()
        if not path.is_relative_to(root):
            continue
        findings.add(
            Finding(tool, str(path.relative_to(root)), int(match["line"]), int(match["col"]), check.removeprefix("-W"), match["message"])
        )
    failed = process.returncode != 0 and not findings
    return findings, (process.stderr.strip() if failed else "")


def escape(value: str, is_property: bool = False) -> str:
    value = value.replace("%", "%25").replace("\r", "%0D").replace("\n", "%0A")
    return value.replace(":", "%3A").replace(",", "%2C") if is_property else value


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--build-dir", required=True, type=Path, help="build directory with compile_commands.json")
    parser.add_argument("--base", required=True, help="commit to compare the working tree to")
    parser.add_argument("--clang-tidy", default="clang-tidy")
    parser.add_argument("--clazy", default="clazy-standalone")
    parser.add_argument("--clazy-checks", default="level1")
    parser.add_argument("--jobs", type=int, default=os.cpu_count())
    args = parser.parse_args()

    root = Path(git("rev-parse", "--show-toplevel").strip()).resolve()
    build_dir = args.build_dir.resolve()
    changed = changed_lines(args.base)
    units = translation_units(build_dir, root, changed)
    print(f"{len(changed)} changed C++ files, analyzing {len(units)} translation units", flush=True)

    commands = []
    for unit in units:
        commands.append(("clang-tidy", [args.clang_tidy, "-p", str(build_dir), "--quiet", str(unit)]))
        commands.append(("clazy", [args.clazy, "-p", str(build_dir), f"-checks={args.clazy_checks}", str(unit)]))

    findings: set[Finding] = set()
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for (tool, command), (result, error) in zip(commands, pool.map(lambda c: run_tool(c[0], c[1], root), commands)):
            findings |= result
            if error:
                print(f"{tool} failed on {command[-1]}:\n{error}", file=sys.stderr)

    new = sorted((f for f in findings if f.line in changed.get(f.file, ())), key=lambda f: (f.file, f.line, f.col, f.tool))
    in_actions = os.environ.get("GITHUB_ACTIONS") == "true"
    for f in new:
        if in_actions:
            title = escape(f"{f.tool}: {f.check}", True)
            print(f"::warning file={escape(f.file, True)},line={f.line},col={f.col},title={title}::{escape(f.message)}")
        else:
            print(f"{f.file}:{f.line}:{f.col}: {f.message} [{f.tool}: {f.check}]")

    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a") as out:
            out.write(f"### Static analysis of changed code\n\n{len(new)} new findings in {len(units)} analyzed translation units.\n")
            if new:
                out.write("\n| Location | Tool | Check | Message |\n|---|---|---|---|\n")
                for f in new:
                    message = f.message.replace("|", "\\|")
                    out.write(f"| `{f.file}:{f.line}` | {f.tool} | `{f.check}` | {message} |\n")

    print(f"{len(new)} new findings ({len(findings) - len(new)} more in unchanged lines of the analyzed files)")
    return 1 if new else 0


if __name__ == "__main__":
    sys.exit(main())
