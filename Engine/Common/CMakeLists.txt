find_package(OpenGL REQUIRED)

add_library(PandaEmpty
	Entry/main.cpp
	Entry/BaseApplication.hpp
	Entry/BaseApplication.cpp
	Entry/EmptyApplication.hpp
	Entry/EmptyApplication.cpp
	Entry/EmptyApplicationConfig.cpp
	)

IF(${WIN32})
	add_library(PandaD3D12
		Entry/main.cpp
		Entry/BaseApplication.hpp
		Entry/BaseApplication.cpp
		Entry/WindowsApplication.hpp
		Entry/WindowsApplication.cpp
		Entry/WindowsD3D12Application.hpp
		Entry/WindowsD3D12Application.cpp
		Entry/WindowsD3D12ApplicationConfig.cpp
		)

	target_link_libraries(PandaD3D12
		Core
		SceneManager
		D3D12RHI
		InputManager
		DebugManager
		${CROSSGUID_D_LIB}
		${OPENDDL_LIB}
		${OPENGEX_LIB}
		${ZLIB_LIB}
		d2d1
		d3d12
		dxgi
		d3dcompiler
	)
		
	add_library(PandaOpenGL
		Entry/main.cpp
		Entry/BaseApplication.hpp
		Entry/BaseApplication.cpp
		Entry/WindowsApplication.hpp
		Entry/WindowsApplication.cpp
		Entry/WindowsOpenGLApplication.hpp
		Entry/WindowsOpenGLApplication.cpp
		Entry/WindowsOpenGLApplicationConfig.cpp
		${PROJECT_SOURCE_DIR}/Engine/RHI/OpenGL/glad/src/glad_wgl.c
		)

	target_include_directories(PandaOpenGL 
		PRIVATE 
		"${PROJECT_SOURCE_DIR}/Engine/RHI/OpenGL/glad/include")

	target_link_libraries(PandaOpenGL
		Core
		SceneManager
		OpenGLRHI
		InputManager
		DebugManager
		${CROSSGUID_D_LIB}
		${OPENDDL_LIB}
		${OPENGEX_LIB}
		${ZLIB_LIB}
		${OPENGL_LIBRARIES}
	)
ELSE (${WIN32})
	add_library(PandaOpenGL
		Entry/main.cpp
		Entry/BaseApplication.hpp
		Entry/BaseApplication.cpp
		Entry/LinuxOpenGLApplication.hpp
		Entry/LinuxOpenGLApplication.cpp
		Entry/LinuxOpenGLApplicationConfig.cpp

		${PROJECT_SOURCE_DIR}/Engine/RHI/OpenGL/glad/src/glad_glx.c
		)

	target_include_directories(PandaOpenGL 
		PRIVATE 
		"${PROJECT_SOURCE_DIR}/Engine/RHI/OpenGL/glad/include")
ENDIF(${WIN32}) 