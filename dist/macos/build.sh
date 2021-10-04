#!/bin/bash

mkdir build_macos
cd build_macos
$Qt6_DIR/bin/qmake .. IMGUR_API_KEY=$IMGUR_API_KEY VERSION=$TAG_NAME
make -j2
