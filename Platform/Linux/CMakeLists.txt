add_executable(PandaOpenGL XcbApplication.cpp OpenGLApplication.cpp
	${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_glx.c
    )

find_library(XCB_LIBRARY xcb)
find_library(X11_LIBRARY X11)
find_library(X11_XCB_LIBRARY X11-xcb)
find_library(OPENGL_LIBRARY GL)
find_library(DL_LIBRARY dl)

target_link_libraries(PandaOpenGL OpenGLRHI Common ${OPENGEX_LIBRARY} ${OPENDDL_LIBRARY} ${XG_LIBRARY} ${XCB_LIBRARY} ${X11_LIBRARY} ${X11_XCB_LIBRARY} ${OPENGL_LIBRARY} ${DL_LIBRARY})
target_include_directories(PandaOpenGL PRIVATE "${PROJECT_SOURCE_DIR}/External/src/glad/include")
__add_xg_platform_dependencies(PandaOpenGL)