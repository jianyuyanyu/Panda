@echo off
git submodule update --init External/src/crossguid
mkdir External\build\crossguid
cd External\build\crossguid
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../../src/crossguid
cmake --build . --config release --target install