project(SQLeast)

include_directories(../include)

add_executable(testpagefs testpagefs.cpp)
target_link_libraries(testpagefs pagefs)

add_executable(testrm testrm.cpp)
target_link_libraries(testrm pagefs sqleast-rm)

add_executable(testix testix.cpp)
target_link_libraries(testix pagefs sqleast-rm sqleast-ix)

add_executable(testsm testsm.cpp)
target_link_libraries(testsm pagefs sqleast-rm sqleast-ix sqleast-sm boost_system boost_filesystem)
