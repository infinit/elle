PROJECT(passport CXX)

ADD_EXECUTABLE(8passport
  Passport.cc
)

TARGET_LINK_LIBRARIES(8passport infinit elle nucleus lune etoile hole ${Boost_LIBRARIES})
