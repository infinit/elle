project(passport CXX)

add_executable(8passport
  Passport.cc
)
target_link_libraries(
	8passport
	${INFINIT_LIBRARIES}
)
