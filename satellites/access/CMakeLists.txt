PROJECT(access CXX)

ADD_EXECUTABLE(8access
  Access.cc
)

TARGET_LINK_LIBRARIES(8access infinit elle lune etoile agent hole ${Boost_LIBRARIES})
