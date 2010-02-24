QT4_WRAP_CPP(test-network-door_MOCS
  Server.hh
  Client.hh
)

ADD_EXECUTABLE(test-network-door
  Test.cc
  Server.cc
  Client.cc

  ${test-network-door_MOCS}
)

TARGET_LINK_LIBRARIES(test-network-door elle
                      ${QT_QTNETWORK_LIBRARIES}
                      ${QT_QTCORE_LIBRARIES})
