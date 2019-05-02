# Asset load test
add_executable(AssetLoaderTest AssetLoaderTest.cpp)

target_link_libraries(AssetLoaderTest Common)

add_test(NAME TEST_AssetLoader COMMAND AssetLoaderTest)

# panda math test
add_executable(PandaMathTest PandaMathTest.cpp)

target_link_libraries(PandaMathTest
    PandaMath
    Common)

add_test(NAME TEST_PandaMath COMMAND PandaMathTest)

# texture load test
add_executable(TextureLoadTest TextureLoadTest.cpp)
target_link_libraries(TextureLoadTest WindowsApplication Common D2DRHI d2d1)

add_test(NAME TEST_TextureLoad COMMAND TextureLoadTest)
