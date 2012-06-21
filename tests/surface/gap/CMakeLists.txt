
simple_test(gap "" "tests done." "")
target_link_libraries(test_gap  gap elle pthread ${Boost_LIBRARIES})
