# get all header files and source files
FILE(GLOB EDITOR_HEADERS ./*.hpp)
FILE(GLOB EDITOR_SOURCES ./*.cpp)

add_executable(EditorOGL
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS})
	
target_link_libraries(EditorOGL
	PandaOpenGL
	)

add_executable(EditorD3D
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS})
	
target_link_libraries(EditorD3D
	PandaD3D12
	)
