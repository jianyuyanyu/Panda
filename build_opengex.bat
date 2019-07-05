@echo off
::git submodule update --init 3rdParty\OpenGEX
mkdir 3rdParty\OpenGEX\build
cd 3rdParty\OpenGEX\build
rm -rf *
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../
cmake --build . --config debug --target install