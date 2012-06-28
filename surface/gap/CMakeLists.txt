
file(GLOB gap_srcs
  gap.cc
  State.cc
)

add_library(gap SHARED ${gap_srcs})
