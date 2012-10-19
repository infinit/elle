project(transfer CXX)

add_executable(8transfer
  Transfer.cc
)
target_link_libraries(
	8transfer
	${INFINIT_LIBRARIES}
)
