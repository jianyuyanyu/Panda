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
    target_link_libraries(TextureLoadTest WindowsApplication Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY} D2DRHI d2d1)
    __add_xg_platform_dependencies(TextureLoadTest)
    add_test(NAME TEST_TextureLoad COMMAND TextureLoadTest)
ENDIF(${WIN32})

# scene object test
#include(PlatformDependencies)
#add_executable(SceneObjectTest SceneObjectTest.cpp)
#target_link_libraries(SceneObjectTest Common ${XG_LIBRARY})
#__add_xg_platform_dependencies(SceneObjectTest)
#add_test(NAME TEST_SceneObject COMMAND SceneObjectTest)

# scene parser test
add_executable(OgexParserTest OgexParserTest.cpp)
target_link_libraries(OgexParserTest Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY})
__add_xg_platform_dependencies(OgexParserTest)
add_test(NAME TEST_OgexParser COMMAND OgexParserTest)

# scene loading test
add_executable(SceneLoadingTest SceneLoadingTest.cpp)
target_link_libraries(SceneLoadingTest Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY})
__add_xg_platform_dependencies(SceneLoadingTest)
add_test(NAME TEST_SceneLoading COMMAND SceneLoadingTest)

# color space conversion
add_executable(ColorSpaceConversionTest ColorSpaceConversionTest.cpp)
target_link_libraries(ColorSpaceConversionTest PandaMath)
add_test(NAME Test_ColorSpaceConversion COMMAND ColorSpaceConversionTest)

# Jpeg parser test
add_executable(JpegParserTest JpegParserTest.cpp)
target_link_libraries(JpegParserTest Common)
add_test(NAME TEST_JpegParser COMMAND JpegParserTest)