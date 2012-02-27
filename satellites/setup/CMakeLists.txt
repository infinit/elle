PROJECT(setup CXX)

ADD_EXECUTABLE(8setup
  Setup.cc
)

TARGET_LINK_LIBRARIES(8setup infinit elle lune etoile)
