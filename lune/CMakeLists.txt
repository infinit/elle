PROJECT(Lune CXX)

ADD_LIBRARY(lune
  Associat.cc
  Authority.cc
  Chain.cc
  Identity.cc
  Lune.cc
  Memento.cc
  Passport.cc
  Phrase.cc
)

TARGET_LINK_LIBRARIES(lune elle etoile)
