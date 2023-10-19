#!/bin/bash
PROBABLY_TAG=${GITHUB_REF/refs\/tags\//}

if [[ "${PROBABLY_TAG}" == "" ]]; then
    PROBABLY_TAG=$(git describe --tags)
fi
TAG=$(grep "^v[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]*$" <<< "$PROBABLY_TAG" | sed "s/^v//")

# Default tag
#if [ -z "$TAG" ]; then
#fi

echo $TAG
exit 0
