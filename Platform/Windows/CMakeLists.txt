add_executable(PandaOpenGL
	WindowsApplication.cpp
	OpenGLApplication.cpp
	${PROJECT_SOURCE_DIR}/External/GL/src/glad_wgl.c
	)
target_link_libraries(PandaOpenGL
	Common
	OpenGLRHI
	opengl32
	)
target_include_directories(PandaOpenGL
	PRIVATE "${PROJECT_SOURCE_DIR}/External/GL/include")
	

add_executable (PandaD3D12 
	WindowsApplication.cpp
	D3D12Application.cpp
	)
	
target_link_libraries (PandaD3D12 
	Common
	D3DRHI
	d3d12
	dxgi
	d3dcompiler
	)
	
