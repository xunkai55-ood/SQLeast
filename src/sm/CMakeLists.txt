project(SQLeast)

include_directories(../../include)

aux_source_directory(. SRC_LIST)

message("SM: ${SRC_LIST}")
add_library(sqleast-sm STATIC ${SRC_LIST})
