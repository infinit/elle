project(network CXX)

add_executable(8network
  Network.cc
)
target_link_libraries(
	8network
	${INFINIT_LIBRARIES}
)
