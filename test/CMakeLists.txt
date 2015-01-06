project(SQLeast)

include_directories(../include)

add_executable(testpagefs testpagefs.cpp)
target_link_libraries(testpagefs pagefs)

add_executable(testrm testrm.cpp)
target_link_libraries(testrm pagefs sqleast-rm)
