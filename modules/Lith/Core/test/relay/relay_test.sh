#!/usr/bin/env bash
# Wire-level tests of both WeeChat relay protocols against a throwaway local weechat-headless.
# Exits 77 (ctest SKIP) when the required tools are missing.

set -u

PASSWORD="lith-test-secret"
API_PORT="${LITH_TEST_API_PORT:-$((20000 + RANDOM % 10000))}"
WEECHAT_PORT="${LITH_TEST_WEECHAT_PORT:-$((API_PORT + 1))}"

for tool in weechat-headless curl openssl sha256sum sha512sum base64 timeout; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "SKIP: $tool not found"
        exit 77
    fi
done

WORKDIR="$(mktemp -d -t lith-relay-test.XXXXXX)"
WEECHAT_PID=""
cleanup() {
    if [[ -n "$WEECHAT_PID" ]]; then
        kill "$WEECHAT_PID" 2>/dev/null
        wait "$WEECHAT_PID" 2>/dev/null
    fi
    rm -rf "$WORKDIR"
}
trap cleanup EXIT

# relay.network.ipv6 must be off, otherwise WeeChat binds an AF_INET6 socket and rejects an IPv4 bind_address
cat > "$WORKDIR/relay.conf" <<EOF
[network]
bind_address = "127.0.0.1"
ipv6 = off
password = "$PASSWORD"

[port]
api = $API_PORT
weechat = $WEECHAT_PORT
EOF

weechat-headless --dir "$WORKDIR" -P relay --stdout </dev/null >"$WORKDIR/weechat.log" 2>&1 &
WEECHAT_PID=$!

API="http://127.0.0.1:$API_PORT/api"
WS="ws://127.0.0.1:$API_PORT/api"

for _ in $(seq 50); do
    curl -s -m 1 -o /dev/null -u "plain:$PASSWORD" "$API/version" && break
    sleep 0.1
done

FAILURES=0
TESTS=0

pass() { TESTS=$((TESTS + 1)); echo "PASS: $1"; }
fail() { TESTS=$((TESTS + 1)); FAILURES=$((FAILURES + 1)); echo "FAIL: $1"; [[ $# -gt 1 ]] && echo "      $2"; }

# expect_contains <name> <haystack> <needle>
expect_contains() {
    if [[ "$2" == *"$3"* ]]; then pass "$1"; else fail "$1" "expected to find '$3' in: ${2:0:400}"; fi
}

# http_code <curl args...>: prints "<status> <body>"
http() {
    curl -s -m 5 -w '\n%{http_code}' "$@"
}
status_of() { tail -n1 <<<"$1"; }
body_of() { sed '$d' <<<"$1"; }

hash_auth() {
    local algo="$1" ts hash
    ts="$(date +%s)"
    case "$algo" in
        sha256) hash="$(printf '%s%s' "$ts" "$PASSWORD" | sha256sum | cut -d' ' -f1)"; echo "hash:sha256:$ts:$hash" ;;
        sha512) hash="$(printf '%s%s' "$ts" "$PASSWORD" | sha512sum | cut -d' ' -f1)"; echo "hash:sha512:$ts:$hash" ;;
        pbkdf2+sha256 | pbkdf2+sha512)
            local digest="${algo#pbkdf2+}" len=32
            [[ "$digest" == sha512 ]] && len=64
            hash="$(openssl kdf -keylen "$len" -kdfopt "digest:${digest^^}" -kdfopt "pass:$PASSWORD" -kdfopt "salt:$ts" \
                -kdfopt iter:100000 PBKDF2 | tr -d ':' | tr 'A-F' 'a-f')"
            echo "hash:$algo:$ts:100000:$hash"
            ;;
    esac
}

############################################################ API over HTTP

for algo in plain sha256 sha512 pbkdf2+sha256 pbkdf2+sha512; do
    r="$(http -d "{\"password_hash_algo\": [\"$algo\"]}" "$API/handshake")"
    expect_contains "api/http handshake negotiates $algo" "$(body_of "$r")" "\"password_hash_algo\":\"$algo\""
done

r="$(http -u "plain:wrong" "$API/version")"
expect_contains "api/http wrong password is rejected" "$(status_of "$r")" "401"

r="$(http -u "plain:$PASSWORD" "$API/version")"
expect_contains "api/http plain auth" "$(status_of "$r") $(body_of "$r")" "200 {\"weechat_version\""

for algo in sha256 sha512 pbkdf2+sha256 pbkdf2+sha512; do
    r="$(http -u "$(hash_auth "$algo")" "$API/version")"
    expect_contains "api/http $algo auth" "$(status_of "$r")" "200"
done

AUTH=(-u "plain:$PASSWORD")

r="$(http "${AUTH[@]}" "$API/buffers?colors=weechat")"
expect_contains "api/http buffers lists core buffer" "$(body_of "$r")" '"name":"core.weechat"'
# WeeChat serializes ids as doubles, so some come in exponent notation, e.g. 1.79345440458135e+15
CORE_ID="$(body_of "$r" | grep -o '"id":[^,]*,"name":"core.weechat"' | head -1 | sed 's/^"id":\([^,]*\),.*/\1/')"
CORE_ID="$(printf '%.0f' "$CORE_ID" 2>/dev/null)"
if [[ "$CORE_ID" =~ ^[0-9]+$ ]]; then pass "api/http buffer id is numeric"; else fail "api/http buffer id is numeric"; fi

r="$(http "${AUTH[@]}" -X POST -d '{"buffer_name": "core.weechat", "command": "/print lith-http-marker"}' "$API/input")"
expect_contains "api/http input" "$(status_of "$r")" "204"

r="$(http "${AUTH[@]}" "$API/buffers/$CORE_ID/lines?lines=-5&colors=weechat")"
expect_contains "api/http lines by buffer id contain printed text" "$(body_of "$r")" '"message":"lith-http-marker"'

r="$(http "${AUTH[@]}" "$API/buffers/core.weechat/lines?lines=-5&colors=weechat")"
expect_contains "api/http lines by buffer name" "$(body_of "$r")" '"message":"lith-http-marker"'

r="$(http "${AUTH[@]}" "$API/buffers/core.weechat/nicks")"
expect_contains "api/http nicks returns nicklist root" "$(body_of "$r")" '"name":"root"'

r="$(http "${AUTH[@]}" "$API/hotlist")"
expect_contains "api/http hotlist" "$(status_of "$r")" "200"

r="$(http "${AUTH[@]}" -X POST -d '{"data": "lith-ping"}' "$API/ping")"
expect_contains "api/http ping echoes data" "$(body_of "$r")" '"data":"lith-ping"'

r="$(http "${AUTH[@]}" -X POST -d '{"buffer_name": "core.weechat", "command": "/qui"}' "$API/completion")"
expect_contains "api/http completion" "$(body_of "$r")" '"quit"'

r="$(http "${AUTH[@]}" -X POST -d '{}' "$API/sync")"
expect_contains "api/http sync is websocket-only" "$(status_of "$r")" "403"

############################################################ API over websocket

# ws_session <auth> <frames...>: sends each frame (with a pause in between) and prints everything received;
# auth "protocol:<credentials>" authenticates through Sec-WebSocket-Protocol instead of the Authorization header
ws_session() {
    local auth="$1"
    shift
    local auth_args=(-u "$auth")
    if [[ "$auth" == protocol:* ]]; then
        local token
        token="$(printf '%s' "${auth#protocol:}" | base64 -w0 | tr '+/' '-_' | tr -d '=')"
        auth_args=(-H "Sec-WebSocket-Protocol: api.weechat, base64url.bearer.authorization.weechat.$token")
    fi
    # curl doesn't hang up on stdin EOF, so bound the session by the time needed to send all frames
    local deciseconds=$(($# * 2 + 8))
    {
        for frame in "$@"; do
            printf '%s\n' "$frame"
            sleep 0.2
        done
    } | timeout "$((deciseconds / 10)).$((deciseconds % 10))" curl -s -N "${auth_args[@]}" -T - "$WS"
}

out="$(ws_session "plain:wrong" '{"request": "GET /api/version"}')"
if [[ "$out" != *'"code":200'* ]]; then pass "api/ws wrong password is rejected"; else fail "api/ws wrong password is rejected" "$out"; fi

out="$(ws_session "$(hash_auth pbkdf2+sha512)" '{"request": "GET /api/version", "request_id": "ver"}')"
expect_contains "api/ws hashed auth and request_id roundtrip" "$out" '"request_id":"ver","body_type":"version"'

out="$(ws_session "protocol:$(hash_auth sha256)" '{"request": "GET /api/version", "request_id": "ver"}')"
expect_contains "api/ws auth through Sec-WebSocket-Protocol" "$out" '"request_id":"ver","body_type":"version"'

out="$(ws_session "protocol:plain:wrong" '{"request": "GET /api/version"}')"
if [[ "$out" != *'"code":200'* ]]; then pass "api/ws wrong Sec-WebSocket-Protocol password is rejected"; else fail "api/ws wrong Sec-WebSocket-Protocol password is rejected" "$out"; fi

out="$(ws_session "plain:$PASSWORD" \
    '{"request": "GET /api/buffers?lines=-5&nicks=true&colors=weechat", "request_id": "init"}' \
    '{"request": "POST /api/sync", "body": {"colors": "weechat"}}' \
    '{"request": "POST /api/input", "body": {"buffer_name": "core.weechat", "command": "/print lith-ws-marker"}}' \
    '{"request": "GET /api/hotlist", "request_id": "hl"}' \
    '{"request": "POST /api/ping", "body": {"data": "ws-ping"}, "request_id": "png"}')"
expect_contains "api/ws buffers with lines" "$out" '"request_id":"init","body_type":"buffers"'
expect_contains "api/ws buffers include nicklist" "$out" '"nicklist_root":{'
expect_contains "api/ws sync accepted" "$out" '"code":204,"message":"No Content","request":"POST /api/sync"'
expect_contains "api/ws line pushed after sync" "$out" '"event_name":"buffer_line_added"'
expect_contains "api/ws pushed line content" "$out" '"message":"lith-ws-marker"'
expect_contains "api/ws hotlist" "$out" '"request_id":"hl","body_type":"hotlist"'
expect_contains "api/ws ping" "$out" '"request_id":"png","body_type":"ping"'

out="$(ws_session "plain:$PASSWORD" \
    '{"request": "POST /api/sync", "body": {"colors": "weechat"}}' \
    '{"request": "POST /api/input", "body": {"buffer_name": "core.weechat", "command": "/buffer add lith-ws-buffer"}}' \
    '{"request": "POST /api/input", "body": {"buffer_name": "core.weechat", "command": "/buffer close lith-ws-buffer"}}')"
expect_contains "api/ws buffer_opened event" "$out" '"event_name":"buffer_opened"'
expect_contains "api/ws buffer_closed event" "$out" '"event_name":"buffer_closed"'

############################################################ binary weechat protocol

# weechat_session <commands...>: sends newline-terminated commands, prints the raw binary replies
weechat_session() {
    exec 3<>"/dev/tcp/127.0.0.1/$WEECHAT_PORT" || return
    for cmd in "$@"; do
        printf '%s\n' "$cmd" >&3
    done
    # WeeChat executes relayed input on a timer, give pushed lines time to arrive before quitting
    sleep 1
    printf 'quit\n' >&3
    timeout 5 cat <&3 | tr '\0' ' '
    exec 3<&-
}

out="$(weechat_session '(hs) handshake password_hash_algo=plain:sha256:sha512:pbkdf2+sha256:pbkdf2+sha512,compression=off')"
expect_contains "weechat handshake reply" "$out" "password_hash_algo"
expect_contains "weechat handshake negotiates pbkdf2+sha512" "$out" "pbkdf2+sha512"

out="$(weechat_session "init password=wrong,compression=off" '(ver) info version')"
if [[ "$out" != *"4."* ]]; then pass "weechat wrong password is rejected"; else fail "weechat wrong password is rejected" "$out"; fi

out="$(weechat_session "init password=$PASSWORD,compression=off" \
    '(ver) info version' \
    '(handleBufferInitialization) hdata buffer:gui_buffers(*) number,name,short_name,hidden,title,local_variables' \
    '(handleFirstReceivedLine) hdata buffer:gui_buffers(*)/own_lines/last_line(-1)/data' \
    '(handleHotlistInitialization) hdata hotlist:gui_hotlist(*)' \
    'sync' \
    '(handleNicklistInitialization) nicklist' \
    'input core.weechat /print lith-binary-marker' \
    '(pingid) ping 42')"
expect_contains "weechat info version" "$out" "version"
expect_contains "weechat buffer hdata" "$out" "number:int,name:str,short_name:str"
expect_contains "weechat line hdata" "$out" "handleFirstReceivedLine"
expect_contains "weechat hotlist hdata" "$out" "handleHotlistInitialization"
expect_contains "weechat nicklist hdata" "$out" "handleNicklistInitialization"
expect_contains "weechat line pushed after sync" "$out" "_buffer_line_added"
expect_contains "weechat pushed line content" "$out" "lith-binary-marker"
expect_contains "weechat pong" "$out" "_pong"

out="$(weechat_session "init password=$PASSWORD,compression=zlib" '(ver) info version')"
if [[ -n "$out" ]]; then pass "weechat zlib compression replies"; else fail "weechat zlib compression replies"; fi

echo
echo "$((TESTS - FAILURES))/$TESTS passed"
if [[ $FAILURES -ne 0 ]]; then
    echo "--- weechat log ---"
    cat "$WORKDIR/weechat.log"
    exit 1
fi
