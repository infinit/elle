
simple_test(gap "" "tests done." "")
target_link_libraries(test_gap
	gap elle lune infinit metaclient nucleus hole
	pthread ${Boost_LIBRARIES})
