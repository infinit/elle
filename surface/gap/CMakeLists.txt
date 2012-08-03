file(GLOB gap_srcs
  gap.cc
  State.cc
)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")

add_library(gap SHARED ${gap_srcs})

target_link_libraries(gap
	elle common lune infinit metal
	${Boost_LIBRARIES}
	${INFINIT_OPENSSL_SHARED_LIBRARIES}
)

add_subdirectory(python)
