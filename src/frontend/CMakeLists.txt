find_package(BISON)
find_package(FLEX)


BISON_TARGET(MyParser parser.y ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp)
FLEX_TARGET(MyScanner lexer.l  ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)


include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})
add_library(sqleast-frontend STATIC
   nodes.cpp
   ${BISON_MyParser_OUTPUTS}
   ${FLEX_MyScanner_OUTPUTS}
)
target_link_libraries(sqleast-frontend sqleast-ql)
