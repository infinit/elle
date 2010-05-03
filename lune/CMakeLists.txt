PROJECT(Lune CXX)

ADD_LIBRARY(lune
  Lune.cc
  Memento.cc
)

TARGET_LINK_LIBRARIES(lune elle etoile)
