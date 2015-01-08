project(SQLeast)

include_directories(../../include)

aux_source_directory(. SRC_LIST)

message("QL: ${SRC_LIST}")
add_library(sqleast-ql STATIC ${SRC_LIST})
