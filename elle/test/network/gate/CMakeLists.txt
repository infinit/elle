ADD_EXECUTABLE(test-network-gate
  Test.cc
  Server.cc
  Client.cc
)

TARGET_LINK_LIBRARIES(test-network-gate elle
					${QT_QTNETWORK_LIBRARIES}
					${QT_QTCORE_LIBRARIES})
