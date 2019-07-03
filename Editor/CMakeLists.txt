# get all header files and source files
FILE(GLOB EDITOR_HEADERS ./*.hpp)
FILE(GLOB EDITOR_SOURCES ./*.cpp)

add_executable(Editor
    ${EDITOR_SOURCES}
    ${EDITOR_HEADERS})
IF(WIN32)
    target_link_libraries(Editor
                            BulletPhysics
                            PandaGameEngineOpenGL
                        )
    add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/External/Windows/bin/zlib.dll"
            $<TARGET_FILE_DIR:Editor>)
ELSE(WIN32)
    target_link_libraries(Editor
                            BulletPhysics
                            PandaGameEngine
                        )
ENDIF(WIN32)