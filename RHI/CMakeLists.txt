add_subdirectory(Empty)
add_subdirectory(OpenGL)

# If the platform is windows, add DX library
IF(${WIN32})
	# Windows specific code
	add_subdirectory(D3D)
ENDIF(${WIN32})