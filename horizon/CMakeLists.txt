project(Horizon CXX)

add_library(horizon
  Horizon.cc
  Policy.cc
)

add_subdirectory(${INFINIT_PLATFORM_NO_ARCH})
