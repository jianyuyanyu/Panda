# add to math lib
add_library(PandaMath
  PandaMath.cpp)

# add executable for test
add_executable(PandaMathTest
    PandaMathTest.cpp
    PandaMath.cpp
)

#target_link_libraries(PandaMathTest
#    PandaMath
#)
