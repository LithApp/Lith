#!/bin/bash

mkdir build_macos
cd build_macos
$Qt5_Dir/bin/qmake ..
make -j2
