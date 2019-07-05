#!/bin/bash
git submodule update --init 3rdParty/libpng
mkdir -p 3rdParty/libpng/build
cd 3rdParty/libpng/build
cmake -DCMAKE_INSTALL_PREFIX=../../Linux ../
cmake --build . --config debug --target install

