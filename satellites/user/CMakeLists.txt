project(user CXX)

add_executable(8user
  User.cc
)
target_link_libraries(
	8user
	${INFINIT_LIBRARIES}
)
