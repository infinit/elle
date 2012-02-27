PROJECT(shell CXX)

ADD_EXECUTABLE(8shell
  Shell.cc
)

TARGET_LINK_LIBRARIES(8shell infinit elle nucleus lune etoile agent hole)
