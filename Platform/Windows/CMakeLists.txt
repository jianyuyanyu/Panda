add_library(WindowsApplication WindowsApplication.cpp)

add_executable (PandaD3D12 
	D3D12Application.cpp
	)
	
target_link_libraries (PandaD3D12 
	WindowsApplication
	Common
	${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY}
	D3DRHI
	d3d12
	dxgi
	d3dcompiler
	)
__add_xg_platform_dependencies(PandaD3D12)

find_package(OpenGL REQUIRED)

add_executable(PandaOpenGL
	OpenGLApplication.cpp
	${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_wgl.c
	)
target_link_libraries(PandaOpenGL
	WindowsApplication
	Common
	${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY}
	OpenGLRHI
	${OPENGL_LIBRARIES}
	)
target_include_directories(PandaOpenGL
	PRIVATE "${PROJECT_SOURCE_DIR}/External/src/glad/include")
__add_xg_platform_dependencies(PandaOpenGL)
