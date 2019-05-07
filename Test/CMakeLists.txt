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
IF(${WIN32})
	add_executable(TextureLoadTest TextureLoadTest.cpp)
    target_link_libraries(TextureLoadTest WindowsApplication Common D2DRHI d2d1)
    add_test(NAME TEST_TextureLoad COMMAND TextureLoadTest)
ENDIF(${WIN32})

# scene object test
include(PlatformDependencies)
add_executable(SceneObjectTest SceneObjectTest.cpp)
target_link_libraries(SceneObjectTest Common ${XG_LIBRARY})
__add_xg_platform_dependencies(SceneObjectTest)
add_test(NAME TEST_SceneObject COMMAND SceneObjectTest)

# scene parser test
add_executable(OgexParserTest OgexParserTest.cpp)
target_link_libraries(OgexParserTest Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY})
__add_xg_platform_dependencies(OgexParserTest)
add_test(NAME TEST_OgexParser COMMAND OgexParserTest)
