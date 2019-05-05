add_library(WindowsApplication WindowsApplication.cpp)

add_executable (PandaD3D12 
	D3D12Application.cpp
	)
	
target_link_libraries (PandaD3D12 
	WindowsApplication
	Common
	D3DRHI
	d3d12
	dxgi
	d3dcompiler
	)

find_package(OpenGL REQUIRED)

add_executable(PandaOpenGL
	OpenGLApplication.cpp
	${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_wgl.c
	)
target_link_libraries(PandaOpenGL
	WindowsApplication
	Common
	OpenGLRHI
	${OPENGL_LIBRARIES}
	)
target_include_directories(PandaOpenGL
	PRIVATE "${PROJECT_SOURCE_DIR}/External/src/glad/include")
	
