project(Horizon CXX)

add_library(horizon
  Crib.cc
  Crux.cc
  Handle.cc
  Horizon.cc
  Policy.cc
  Ward.cc
)

add_subdirectory(${INFINIT_PLATFORM_NO_ARCH})
