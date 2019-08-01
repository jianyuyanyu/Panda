FILE(GLOB CORE_HEADER ./*.hpp)
FILE(GLOB CORE_SOURCE ./*.cpp)

FILE(GLOB CORE_INTERFACE_HEADER ./Interface/*.hpp)
FILE(GLOB CORE_INTERFACE_SOURCE ./Interface/*.cpp)

FILE(GLOB CORE_MATH_HEADER ./Math/*.hpp)
FILE(GLOB CORE_MATH_SOURCE ./Math/*.cpp)

FILE(GLOB CORE_PARSER_HEADER ./Parser/*.hpp)
FILE(GLOB CORE_PARSER_SOURCE ./Parser/*.cpp)

add_library(Core

	${CORE_INTERFACE_HEADER}
	
	${CORE_MATH_HEADER}
	${CORE_MATH_SOURCE}
	
	${CORE_PARSER_HEADER}
	${CORE_PARSER_SOURCE}
	${CORE_HEADER}
	${CORE_SOURCE})

target_link_libraries(Core
		
		${CROSSGUID_D_LIB}
		${OPENDDL_LIB}
		${OPENGEX_LIB}
		${ZLIB_LIB}
		${OPENGL_LIBRARIES}
		${ASSIMP_LIB}
	)
	
source_group("Header Files" FILES ${CORE_HEADER})
source_group("Header Files\\Interface" FILES ${CORE_INTERFACE_HEADER})
source_group("Header Files\\Math" FILES ${CORE_MATH_HEADER})
source_group("Header Files\\Parser" FILES ${CORE_PARSER_HEADER})

source_group("Source Files\\Math" FILES ${CORE_MATH_SOURCE})
source_group("Source Files\\Parser" FILES ${CORE_PARSER_SOURCE})
source_group("Source Files\\Interface" FILES ${CORE_INTERFACE_SOURCE})
source_group("Source Files" FILES ${CORE_SOURCE})