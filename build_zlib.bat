@echo off
::git submodule update --init 3rdParty\zlib
mkdir 3rdParty\zlib\build
cd 3rdParty\zlib\build
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../
cmake --build . --config release --target install
