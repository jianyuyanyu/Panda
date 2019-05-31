find_package(OpenGL REQUIRED)

# D3D12 Game Engine lib
######################################################################
add_library(PlatformInterface
	WindowsApplication.cpp
	D3D12Application.cpp
	
	WindowsApplication.hpp
	D3D12Application.hpp
	)
target_link_libraries(PlatformInterface
	d2d1
	d3d12
	dxgi
	)

add_library(PandaGameEngineD3D12
	D3D12ApplicationConfig.cpp)
target_link_libraries(PandaGameEngineD3D12
	Common
	D3DRHI
	PlatformInterface)

# OpenGL Game Engine lib
#######################################################################
add_library(PlatformInterfaceOpenGL
                WindowsApplication.cpp
                OpenGLApplication.cpp
                ${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_wgl.c
)
target_link_libraries(PlatformInterfaceOpenGL
                ${OPENGL_LIBRARIES}
)
target_include_directories(PlatformInterfaceOpenGL PRIVATE 
        "${PROJECT_SOURCE_DIR}/External/src/glad/include")

add_library(PandaGameEngineOpenGL
	OpenGLApplicationConfig.cpp)
target_link_libraries(PandaGameEngineOpenGL
	Common
	OpenGLRHI
	PlatformInterfaceOpenGL
)
target_include_directories(PandaGameEngineOpenGL
	PRIVATE
	"${PROJECT_SOURCE_DIR}/External/src/glad/include")