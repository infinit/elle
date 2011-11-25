ADD_EXECUTABLE(test-network-udp EXCLUDE_FROM_ALL
  Test.cc
  Node.cc
  Neighbour.cc
  Table.cc
)

TARGET_LINK_LIBRARIES(test-network-udp elle
				       ${QT_QTNETWORK_LIBRARIES}
				       ${QT_QTCORE_LIBRARIES})
