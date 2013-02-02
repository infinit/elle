project(progress CXX)

add_executable(8progress
  Progress.cc
)
target_link_libraries(
	8progress
	${INFINIT_LIBRARIES}
)
