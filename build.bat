mkdir Build
pushd Build
cmake -G "Visual Studio 15 2017" ..
cmake --build . --config debug
popd