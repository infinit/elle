
file(GLOB gap_srcs
  gap.cc
  State.cc
)

add_library(gap SHARED ${gap_srcs})

if("${INFINIT_PLATFORM}" MATCHES "linux*")
  add_subdirectory(python)
endif()
