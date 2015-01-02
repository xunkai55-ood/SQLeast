project(SQLeast)

include_directories(../include)
add_executable(testpagefs testpagefs.cpp)
target_link_libraries(testpagefs pagefs)