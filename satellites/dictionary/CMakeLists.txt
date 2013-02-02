project(dictionary CXX)

add_executable(8dictionary
  Dictionary.cc
)
target_link_libraries(
	8dictionary
	${INFINIT_LIBRARIES}
)
