FILE(GLOB BILLIARD_HEADERS ./*.hpp)
FILE(GLOB BILLIARD_SOURCES ./*.cpp)

IF (WIN32)
    add_executable(BilliardGameD3D12
        ${BILLIARD_SOURCES}
        ${BILLIARD_HEADERS})

    target_link_libraries(BilliardGameD3D12
        BulletPhysics
        PandaGameEngineD3D12)


    add_executable(BilliardGameOpenGL
        ${BILLIARD_SOURCES}
        ${BILLIARD_HEADERS})

    target_link_libraries(BilliardGameOpenGL
        BulletPhysics
        PandaGameEngineOpenGL)
ELSE(WIN32)
    add_executable(BilliardGameOpenGL
        ${BILLIARD_SOURCES}
        ${BILLIARD_HEADERS})

    target_link_libraries(BilliardGameOpenGL
        BulletPhysics
        PandaGameEngineOpenGL)
ENDIF(WIN32)

IF(WIN32)
    add_custom_command(TARGET BilliardGameD3D12 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/External/Windows/bin/zlib.dll"
            $<TARGET_FILE_DIR:BilliardGameD3D12>)

    add_custom_command(TARGET BilliardGameOpenGL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/External/Windows/bin/zlib.dll"
            $<TARGET_FILE_DIR:BilliardGameOpenGL>)
ENDIF(WIN32)