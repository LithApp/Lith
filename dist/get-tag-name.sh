#!/bin/bash
PROBABLY_TAG=${GITHUB_REF/refs\/tags\//}

TAG=$(grep "^[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]*$" <<< "$PROBABLY_TAG")

# Default tag
#if [ -z "$TAG" ]; then
#fi

echo $TAG
exit 0
