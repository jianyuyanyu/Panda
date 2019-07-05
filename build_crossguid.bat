@echo off
::git submodule update --init 3rdParty/crossguid
mkdir 3rdParty\crossguid\build
cd 3rdParty\crossguid\build
rm -rf *
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../
cmake --build . --config debug --target install