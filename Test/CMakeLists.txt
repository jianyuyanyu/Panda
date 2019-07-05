# Asset load test
add_executable(AssetLoaderTest AssetLoaderTest.cpp)
target_link_libraries(AssetLoaderTest Core)
add_test(NAME TEST_AssetLoader COMMAND AssetLoaderTest)

# panda math test
add_executable(PandaMathTest PandaMathTest.cpp)
target_link_libraries(PandaMathTest
    Core)

add_test(NAME TEST_PandaMath COMMAND PandaMathTest)

# texture load test
#IF(${WIN32})
#	add_executable(TextureLoadTest TextureLoadTest.cpp)
#    target_link_libraries(TextureLoadTest 
#        PandaGameEngine)
#    add_test(NAME TEST_TextureLoad COMMAND TextureLoadTest)
#ENDIF(${WIN32})

# scene object test
#include(PlatformDependencies)
#add_executable(SceneObjectTest SceneObjectTest.cpp)
#target_link_libraries(SceneObjectTest Common ${XG_LIBRARY})
#__add_xg_platform_dependencies(SceneObjectTest)
#add_test(NAME TEST_SceneObject COMMAND SceneObjectTest)

# scene parser test
add_executable(OgexParserTest OgexParserTest.cpp)
target_link_libraries(OgexParserTest 
    ${OPENGEX_LIB} ${OPENDDL_LIB} ${ZLIB_LIB} ${CROSSGUID_D_LIB} Core SceneManager)
add_test(NAME TEST_OgexParser COMMAND OgexParserTest)

# scene loading test
add_executable(SceneLoadingTest SceneLoadingTest.cpp)
target_link_libraries(SceneLoadingTest 
    Core SceneManager ${OPENGEX_LIB} ${OPENDDL_LIB} ${ZLIB_LIB} ${CROSSGUID_D_LIB})
add_test(NAME TEST_SceneLoading COMMAND SceneLoadingTest)

# color space conversion
add_executable(ColorSpaceConversionTest ColorSpaceConversionTest.cpp)
target_link_libraries(ColorSpaceConversionTest Core)
add_test(NAME Test_ColorSpaceConversion COMMAND ColorSpaceConversionTest)

# Jpeg parser test
add_executable(JpegParserTest JpegParserTest.cpp)
target_link_libraries(JpegParserTest Core)
add_test(NAME TEST_JpegParser COMMAND JpegParserTest)

# Png parser test
add_executable(PngParserTest PngParserTest.cpp)
target_link_libraries(PngParserTest Core ${ZLIB_LIB})
add_test(NAME TEST_PngParser COMMAND PngParserTest)

# tga parser test
add_executable(TgaParserTest TgaParserTest.cpp)
target_link_libraries(TgaParserTest Core)
add_test(NAME TEST_TgaParser COMMAND TgaParserTest)

# add D3D12 test
add_executable(D3D12Cube WIN32 
    D3D12Cube.cpp
    
    d3dx12.h)
target_link_libraries(D3D12Cube 
	d3d12
	dxgi
    d3dcompiler
    Core
)
add_test(NAME TEST_D3D12Cube COMMAND D3D12Cube)

# add numerical test
add_executable(NumericalMethodsTest NumericalMethodsTest.cpp)
add_test(NAME TEST_NumericalMethods COMMAND NumericalMethodsTest)

# bezier test
add_executable(BezierCubic1DTest BezierCubic1DTest.cpp)
add_test(NAME TEST_BezierCubic1D COMMAND BezierCubic1DTest)
target_link_libraries(BezierCubic1DTest Core)

# panda math test
add_executable(PureMathTest PureMathTest.cpp)
add_test(NAME TEST_PureMath COMMAND PureMathTest)

# Linear interpolate test
add_executable(LinearInterpolateTest LinearInterpolateTest.cpp)
target_link_libraries(LinearInterpolateTest Core)
add_test(NAME TEST_LinearInterpolate COMMAND LinearInterpolateTest)

add_executable(QRDecompositionTest QRDecompositionTest.cpp)
target_link_libraries(QRDecompositionTest Core)
add_test(NAME TEST_QRDecomposition COMMAND QRDecompositionTest)

add_executable(PolarDecompositionTest PolarDecompositionTest.cpp)
target_link_libraries(PolarDecompositionTest Core)
add_test(NAME TEST_PolarDecomposition COMMAND PolarDecompositionTest)

add_executable(RasterizationTest RasterizationTest.cpp)
target_link_libraries(RasterizationTest Core)
add_test(NAME TEST_Rasterization COMMAND RasterizationTest)