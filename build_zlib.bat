@echo off
git submodule update --init External/src/zlib
mkdir External\build\zlib
cd External\build\zlib
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../../src/zlib
cmake --build . --config release --target install
