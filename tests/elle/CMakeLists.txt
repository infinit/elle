
link_libraries(elle pthread)

simple_test(print "" "tests done." "")
simple_test(log "" "tests done." "")
target_link_libraries(test_log ${Boost_LIBRARIES})
simple_test(format "" "tests done." "")

add_subdirectory(format)
add_subdirectory(network)
add_subdirectory(serialize)
