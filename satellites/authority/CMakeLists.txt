PROJECT(authority CXX)

ADD_EXECUTABLE(8authority
  Authority.cc
)

TARGET_LINK_LIBRARIES(8authority infinit elle lune etoile ${Boost_LIBRARIES})
