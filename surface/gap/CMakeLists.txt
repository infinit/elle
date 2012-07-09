file(GLOB gap_srcs
  gap.cc
  State.cc
)

add_library(gap SHARED ${gap_srcs})

target_link_libraries(gap
	elle lune infinit metal
	${Boost_LIBRARIES}
	${INFINIT_OPENSSL_SHARED_LIBRARIES}
)

if("${INFINIT_PLATFORM}" MATCHES "linux*")
  add_subdirectory(python)
endif()
