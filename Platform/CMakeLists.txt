IF(${WIN32})
	add_subdirectory(Windows)
ELSEIF(${UNIX})
    # Linux specific code
    add_subdirectory(Linux)
ELSE()
    # Empty
    add_subdirectory(Empty)
ENDIF(${WIN32})