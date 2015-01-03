project(SQLeast)

include_directories(../../include)

aux_source_directory(. SRC_LIST)

message("RM: {SRC_LIST}")
add_library(sqleast-rm STATIC ${SRC_LIST})
