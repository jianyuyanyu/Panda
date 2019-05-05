add_executable(PandaOpenGL
    XcbApplication.cpp
    OpenGLApplication.cpp
    ${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_glx.c
)

target_link_libraries(PandaOpenGL 
    OpenGLRHI
    Common
    xcb
    X11
    X11-xcb
    dl
)

target_include_directories(PandaOpenGL
    PRIVATE
    "${PROJECT_SOURCE_DIR}/External/src/glad/include"
)