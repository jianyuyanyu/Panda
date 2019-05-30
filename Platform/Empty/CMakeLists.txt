add_library(PlatformInterface
            EmptyApplication.cpp
            EmptyApplication.hpp
            )
            
add_library(PandaGameEngine
            EmptyApplicationConfig.cpp)
target_link_libraries(PandaGameEngine 
    Common
    PlatformInterface)