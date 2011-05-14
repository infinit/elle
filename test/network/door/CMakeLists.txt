ADD_EXECUTABLE(test-network-door
  Test.cc
  Server.cc
  Client.cc
)

TARGET_LINK_LIBRARIES(test-network-door elle
					${QT_QTNETWORK_LIBRARIES}
					${QT_QTCORE_LIBRARIES})
