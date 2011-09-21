PROJECT(Lune CXX)

ADD_LIBRARY(lune
  Authority.cc
  Configuration.cc
  Descriptor.cc
  Dictionary.cc
  Identity.cc
  Log.cc
  Lune.cc
  Passport.cc
  Phrase.cc
)

TARGET_LINK_LIBRARIES(lune elle nucleus hole)

IF(${UNIX})
  TARGET_LINK_LIBRARIES(lune pig)
ENDIF()
