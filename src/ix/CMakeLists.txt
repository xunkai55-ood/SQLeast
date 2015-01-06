project(SQLeast)

include_directories(../../include)

aux_source_directory(. SRC_LIST)

message("IX: {SRC_LIST}")
add_library(sqleast-ix STATIC ${SRC_LIST})
