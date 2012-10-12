project(access CXX)

add_executable(8access
  Access.cc
)
target_link_libraries(
	8access
	${INFINIT_LIBRARIES}
)
