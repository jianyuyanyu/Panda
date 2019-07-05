@echo off
git submodule update --init 3rdParty\libpng
mkdir 3rdParty\libpng\build
cd 3rdParty\libpng\build
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../
cmake --build . --config debug --target install

