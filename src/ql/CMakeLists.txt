project(SQLeast)

include_directories(../../include)

# aux_source_directory(. SRC_LIST)
set(SRC_LIST interpreter.cpp)

message("QL: ${SRC_LIST}")
add_library(sqleast-ql STATIC ${SRC_LIST})
