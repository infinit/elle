project(authority CXX)

add_executable(8authority
  Authority.cc
)
target_link_libraries(
	8authority
	${INFINIT_LIBRARIES}
)
