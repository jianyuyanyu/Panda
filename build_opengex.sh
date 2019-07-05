#!/bin/bash
git submodule update --init 3rdParty\OpenGEX
mkdir -p 3rdParty\OpenGEX\build
cd 3rdParty\OpenGEX\build
cmake -DCMAKE_INSTALL_PREFIX=../../ ../
cmake --build . --config debug --target install