project(Lune CXX)

add_library(lune
  Authority.cc
  Configuration.cc
  Descriptor.cc
  Dictionary.cc
  Identity.cc
  Log.cc
  Lune.cc
  Passport.cc
  Phrase.cc
  Set.cc
)

add_definitions(-fPIC)

target_link_libraries(lune hole nucleus common horizon)
