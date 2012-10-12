project(setup CXX)

add_executable(8setup
  Setup.cc
)
target_link_libraries(
	8setup
	${INFINIT_LIBRARIES}
)
