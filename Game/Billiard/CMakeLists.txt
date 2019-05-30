add_executable(BilliardGame
    BilliardGameLogic.cpp
    BilliardGameConfig.cpp)

target_link_libraries(BilliardGame
    BulletPhysics
    PandaGameEngine)

IF(WIN32)
    add_custom_command(TARGET BilliardGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/External/Windows/bin/zlib.dll"
            $<TARGET_FILE_DIR:BilliardGame>)
ENDIF(WIN32)