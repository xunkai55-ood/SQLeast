project(SQLeast)

include_directories(../../include)

aux_source_directory(. SRC_LIST)

message("CLI: ${SRC_LIST}")
add_executable(sqleast ${SRC_LIST})
target_link_libraries(sqleast pagefs sqleast-rm sqleast-ix sqleast-sm sqleast-ql boost_system boost_filesystem)
