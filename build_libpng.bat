@echo off
git submodule update --init External/src/libpng
mkdir External\build\libpng
cd External\build\libpng
cmake -DCMAKE_INSTALL_PREFIX=../../Windows -G "Visual Studio 15 2017" ../../src/libpng
cmake --build . --config debug --target install

