# get all header files and source files
FILE(GLOB COMMON_HEADERS ./*.hpp)
FILE(GLOB INTERFACE_HEADERS ../Interface/*.hpp)
FILE(GLOB PARSER_HEADERS ../Parser/*.hpp)
FILE(GLOB COMMON_SOURCES ./*.cpp)

# add to Common library
add_library(Common
	${COMMON_SOURCES}
	${COMMON_HEADERS}
	${INTERFACE_HEADERS}
	${PARSER_HEADERS}
  )

source_group("Source Files\\Common" FILES ${COMMON_SOURCES})
source_group("Header Files\\Common" FILES ${COMMON_HEADERS})
source_group("Header Files\\Interface" FILES ${INTERFACE_HEADERS})
source_group("Header Files\\Parser" FILES ${PARSER_HEADERS})

target_link_libraries(Common 
    PandaMath
    ${OPENGEX_LIBRARY} 
    ${OPENDDL_LIBRARY} 
    ${XG_LIBRARY} 
    ${ZLIB_LIBRARY}
    ${BULLET_DYNAMICS_LIBRARY} 
    ${BULLET_COLLISION_LIBRARY} 
    ${BULLET_LINEARMATH_LIBRARY}
    )

__add_xg_platform_dependencies(Common)
