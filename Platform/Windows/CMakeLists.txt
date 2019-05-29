find_package(OpenGL REQUIRED)

add_library(PlatformInterface
	WindowsApplication.cpp
	D3D12Application.cpp
	OpenGLApplication.cpp
	${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_wgl.c)
target_link_libraries(PlatformInterface
	d2d1
	d3d12
	dxgi
	${OPENGL_LIBRARIES})
target_include_directories(PlatformInterface 
	PRIVATE
	"${PROJECT_SOURCE_DIR}/External/src/glad/include")

add_library(PandaGameEngineD3D12
	D3D12ApplicationConfig.cpp)
target_link_libraries(PandaGameEngineD3D12
	Common
	D3DRHI
	PlatformInterface)

add_library(PandaGameEngine
	OpenGLApplicationConfig.cpp)
target_link_libraries(PandaGameEngine
	Common
	OpenGLRHI
	PlatformInterface
)
target_include_directories(PandaGameEngine
	PRIVATE
	"${PROJECT_SOURCE_DIR}/External/src/glad/include")