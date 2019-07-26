@echo off
git submodule update --init 3rdParty/assimp
mkdir 3rdParty\assimp\build
cd 3rdParty\assimp\build
rm -rf *
cmake -DCMAKE_INSTALL_PREFIX=../../ -G "Visual Studio 15 2017" ../
cmake --build . --config debug --target install