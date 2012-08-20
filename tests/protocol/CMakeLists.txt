link_libraries(
	protocol
	elle
    ${Boost_LIBRARIES}
)

simple_test(rpc "" "tests done." "")
