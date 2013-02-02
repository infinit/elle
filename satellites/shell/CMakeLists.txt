project(shell CXX)

add_executable(8shell
  Shell.cc
)
target_link_libraries(
	8shell
	${INFINIT_LIBRARIES}
)
