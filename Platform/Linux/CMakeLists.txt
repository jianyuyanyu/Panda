find_library(XCB_LIBRARY xcb)
find_library(X11_LIBRARY X11)
find_library(X11_XCB_LIBRARY X11-xcb)
find_library(OPENGL_LIBRARY GL)
find_library(DL_LIBRARY dl)

add_library(PlatformInterface
    XcbApplication.cpp
    OpenGLApplication.cpp
    ${PROJECT_SOURCE_DIR}/External/src/glad/src/glad_glx.c)
target_link_libraries(PlatformInterface
    ${XCB_LIBRARY}
    ${X11_LIBRARY}
    ${X11_XCB_LIBRARY}
    ${OPENGL_LIBRARY}
    ${DL_LIBRARY}
    }
target_include_directories(PlatformInterface
    PRIVATE 
    "${PROJECT_SOURCE_DIR}/External/src/glad/include")

add_library(PandaGameEngine
    OpenGLApplicationConfig.cpp)
target_link_libraries(PandaGameEngine
    Common
    OpenGLRHI
    PlatformInterface)
target_include_directories(PandaGameEngine
    PRIVATE
    "${PROJECT_SOURCE_DIR}/External/src/glad/include")