#!/bin/bash
mkdir -p Build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config debug
