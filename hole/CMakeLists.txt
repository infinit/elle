PROJECT(Hole CXX)

QT4_WRAP_CPP(Hole_MOCS
  DHT.hh
  DHTRequest.hh
  DHTJoinRequest.hh
  Node.hh
  LocalNode.hh
  RemoteNode.hh)

ADD_LIBRARY(hole
  DHT.cc DHT.hh
  DHTRequest.cc DHTRequest.hh
  DHTJoinRequest.cc DHTJoinRequest.hh
  FingerTable.cc FingerTable.hh
  FreeList.hh FreeList.hxx
  FullTag.cc FullTag.hh
  Key.cc Key.hh Key.hxx
  Node.cc Node.hh
  LocalNode.cc LocalNode.hh
  RemoteNode.cc RemoteNode.hh

  protocol/Header.cc protocol/Header.hh

  ${Hole_MOCS})

ADD_EXECUTABLE(hole-test main.cc)
TARGET_LINK_LIBRARIES(hole-test hole
  ${QT_QTNETWORK_LIBRARIES}
  ${QT_QTCORE_LIBRARIES})

ADD_EXECUTABLE(hole-freelist-test FreeListTest.cc)

ADD_SUBDIRECTORY(shell-node)
