PROJECT(Lune CXX)

ADD_LIBRARY(lune
  Authority.cc
  Identity.cc
  Lune.cc
  Memento.cc
  Phrase.cc
)

TARGET_LINK_LIBRARIES(lune elle etoile)
