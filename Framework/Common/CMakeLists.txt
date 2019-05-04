# 编译成Common库
add_library(Common
  Allocator.cpp
  AssetLoader.cpp
  BaseApplication.cpp
  GraphicsManager.cpp
  MemoryManager.cpp
  SceneManager.cpp
  main.cpp)

target_link_libraries(Common PandaMath)
find_library(XG_LIBRARY xg PATHS ${PANDA_EXTERNAL_LIBRARY_PATH})
target_link_libraries(Common ${XG_LIBRARY})


