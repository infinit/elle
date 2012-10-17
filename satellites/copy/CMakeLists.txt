project(copy CXX)

add_executable(8copy
  Copy.cc
)
target_link_libraries(
	8copy
	${INFINIT_LIBRARIES}
)
