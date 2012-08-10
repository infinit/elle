PROJECT(group CXX)

ADD_EXECUTABLE(8group
  Group.cc
)

TARGET_LINK_LIBRARIES(8group infinit elle lune etoile agent hole ${Boost_LIBRARIES})
