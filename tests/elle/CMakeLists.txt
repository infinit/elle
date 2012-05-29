
<<<<<<< HEAD
link_libraries(elle pthread)

simple_test(print "" "tests done." "")
simple_test(log "" "tests done." "")
target_link_libraries(test_log ${Boost_LIBRARIES})
simple_test(format "" "tests done." "")
=======
link_libraries(
	elle
	pthread
	boost_system
	boost_signals
)
>>>>>>> hotgloupi/serialize

add_subdirectory(cryptography)
add_subdirectory(format)
<<<<<<< HEAD
add_subdirectory(network)
=======
add_subdirectory(io)
>>>>>>> hotgloupi/serialize
add_subdirectory(serialize)
add_subdirectory(standalone)
