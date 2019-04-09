# 这是用来注释的
# 最新的文档网址是：https://cmake.org/cmake/help/latest/index.html
cmake_minimum_required (VERSION 3.1)	# 最低CMake的版本，3.1

# 通过set命令使用CMake变量，set可以看做是对变量的赋值
# 关于变量的信息可以参考这里：https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/Useful-Variables
set (CMAKE_C_COMPILER				"clang-cl")		# 将C编译器指定为clang-cl
set (CMAKE_C_FLAGS					"-Wall") 		# 所有模式下的共同编译参数，显示所有的警告信息
set (CMAKE_C_FLAGS_DEBUG			"/Debug") 		# Debug编译模式下的编译参数
set (CMAKE_C_FLAGS_MINSIZEREL		"-Os -DNDEBUG") # MinSizeRel编译模式下的编译参数
													# -Os相当于-O2.5。是使用了所有-O2的优化选项，但又不缩减代码尺寸的方法。
													# -DNDEBUG是告诉编译器在每个translation unit中定义macro NDEBUG，进而导致所有assert()都被关闭！
set (CMAKE_C_FLAGS_RELEASE			"-O4 -DNDEBUG") # Release编译模式下的编译参数，
													# O4表示最高级别优化，
													# -DNDEBUG是告诉编译器在每个translation unit中定义macro NDEBUG，进而导致所有assert()都被关闭！
set (CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 /Debug")	# RelWithDebInfo编译模式下的编译参数
													# -O2表示2级优化，并且需要调试信息
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_COMPILER             "clang-cl")		# 设置C++编译器为clang-cl
set (CMAKE_CXX_FLAGS                "-Wall -Xclang -std=gnu++14") 	# -Xclang是设置XCode的编译选项
																	# -std=gnu++14表示使用gnu++14的标准
set (CMAKE_CXX_FLAGS_DEBUG          "/Debug")
set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 /Debug")
#set (CMAKE_CXX_STANDARD 11)

project (Panda) # 项目名

include_directories("${PROJECT_SOURCE_DIR}/Framework/Common")		# 包含目录Common
include_directories("${PROJECT_SOURCE_DIR}/Framework/Interface")	# 包含目录Interface

add_subdirectory(Framework)		# 子目录，有一种引用库的感觉，因为子目录中也必须要有CMakeLists.txt文件
add_subdirectory(Empty)
