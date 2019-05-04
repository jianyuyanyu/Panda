@echo off
git submodule update --init External/src/llvm External/src/clang 
mkdir External\build\llvm
cd External\build\llvm
cmake -DCMAKE_INSTALL_PREFIX=../../Windows -DLLVM_ENABLE_PROJECTS=clang -G "Visual Studio 15 2017" ../../src/llvm
cmake --build . --config release --target install