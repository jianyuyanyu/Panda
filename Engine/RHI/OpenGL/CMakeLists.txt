FILE(GLOB OPENGL_RHI_HEADER ./*.hpp)
FILE(GLOB OPENGL_RHI_SOURCE ./*.cpp)

add_library(OpenGLRHI

	../GraphicsManager.hpp
	../GraphicsManager.cpp
	../ShaderModule.hpp
	${OPENGL_RHI_HEADER}
	${OPENGL_RHI_SOURCE}
	glad/src/glad.c
	)
target_include_directories(OpenGLRHI PRIVATE "glad/include")