
link_libraries(
	elle
	pthread
	boost_system
	boost_signals
)

add_subdirectory(cryptography)
add_subdirectory(format)
add_subdirectory(io)
add_subdirectory(serialize)
add_subdirectory(standalone)
