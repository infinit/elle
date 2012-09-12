file(GLOB gap_srcs
  gap.cc
  State.cc
)

add_library(gap SHARED ${gap_srcs})

target_link_libraries(gap LINK_PRIVATE ${INFINIT_LIBRARIES})
add_subdirectory(python)
