link_libraries(
	elle hole lune nucleus infinit
	pthread
    ${Boost_LIBRARIES}
)

simple_test(nn_Object "" "tests done." "")
simple_test(nn_Record "" "tests done." "")
simple_test(nn_Group "" "tests done." "")
