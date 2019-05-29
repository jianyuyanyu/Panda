add_library(PlatformInterface
            EmptyApplication.cpp)
add_library(PandaGameEngine
            EmptyApplicationConfig.cpp)
target_link_libraries(PandaGameEngine 
    Common
    PlatformInterface)