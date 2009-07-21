PROJECT(Hole CXX)

QT4_WRAP_CPP(Hole_MOCS
  DHT.hh)

ADD_LIBRARY(hole
  DHT.cc

  ${Hole_MOCS})
