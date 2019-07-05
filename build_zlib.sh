#!/bin/bash
git submodule update --init 3rdParty/zlib
mkdir -p 3rdParty/zlib/build
cd 3rdParty/zlib/build
cmake -DCMAKE_INSTALL_PREFIX=../../ ../
cmake --build . --config debug --target install