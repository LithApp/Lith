#!/bin/bash

set -x

mkdir build_wasm
pushd build_wasm

${Qt6_DIR}/bin/qmake -r .. CONFIG+=debug IMGUR_API_KEY=$IMGUR_API_KEY
make -j5

