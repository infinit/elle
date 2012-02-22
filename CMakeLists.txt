PROJECT(Comet CXX)

add_definitions(-fPIC)
add_library(comet
  BN.cc
  RAND.cc
  RSA.cc
)

target_link_libraries(comet)
