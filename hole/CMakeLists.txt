PROJECT(Hole CXX)

QT4_WRAP_CPP(Hole_MOCS
  DHT.hh
  LocalNode.hh
  RemoteNode.hh)

ADD_LIBRARY(hole
  DHT.cc DHT.hh
  FreeList.hh FreeList.hxx
  Key.hh
  Node.cc Node.hh
  LocalNode.cc LocalNode.hh
  RemoteNode.hh

  ${Hole_MOCS})

ADD_EXECUTABLE(hole-test main.cc)
TARGET_LINK_LIBRARIES(hole-test hole ${QT_QTCORE_LIBRARIES})

ADD_EXECUTABLE(hole-freelist-test FreeListTest.cc)
