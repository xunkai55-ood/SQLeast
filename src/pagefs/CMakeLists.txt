project(SQLeast)

include_directories(../../include)

aux_source_directory(. SRC_LIST)

message("PAGEFS: {SRC_LIST}")
add_library(pagefs STATIC ${SRC_LIST})
