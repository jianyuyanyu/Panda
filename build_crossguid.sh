#!/bin/bash
git submodule update --init 3rdParty/crossguid
mkdir -p 3rdParty/crossguid/build
cd 3rdParty/crossguid/build
cmake -DCMAKE_INSTALL_PREFIX=../../ ../
cmake --build . --config debug --target install