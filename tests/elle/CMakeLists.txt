
link_libraries(elle pthread)

simple_test(print "" "tests done." "")

add_subdirectory(format)
add_subdirectory(network)
add_subdirectory(serialize)
