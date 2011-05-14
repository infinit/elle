ADD_EXECUTABLE(test-network-slot
  Test.cc
  Node.cc
  Neighbour.cc
  Table.cc
)

TARGET_LINK_LIBRARIES(test-network-slot elle
					${QT_QTNETWORK_LIBRARIES}
					${QT_QTCORE_LIBRARIES})
