# 编译成Common库
add_library(Common
  Allocator.cpp
  MemoryManager.cpp
  BaseApplication.cpp
  GraphicsManager.cpp
  AssetLoader.cpp
  main.cpp)

target_link_libraries(Common PandaMath)
