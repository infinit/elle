PROJECT(Hole CXX)

QT4_WRAP_CPP(Hole_MOCS
  DHT.hh)

ADD_LIBRARY(hole
  DHT.cc

  ${Hole_MOCS})

ADD_EXECUTABLE(hole-test main.cc)
TARGET_LINK_LIBRARIES(hole-test hole ${QT_QTCORE_LIBRARIES})

ADD_EXECUTABLE(hole-freelist-test FreeListTest.cc)