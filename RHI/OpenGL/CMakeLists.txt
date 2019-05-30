add_library(OpenGLRHI
	OpenGLGraphicsManager.cpp
	${PROJECT_SOURCE_DIR}/External/src/glad/src/glad.c

	OpenGLGraphicsManager.hpp
	)
target_include_directories(OpenGLRHI PRIVATE "${PROJECT_SOURCE_DIR}/External/src/glad/include")
add_dependencies(OpenGLRHI PandaMath)