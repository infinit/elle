ADD_EXECUTABLE(test-network-local EXCLUDE_FROM_ALL
  Test.cc
  Server.cc
  Client.cc
)

TARGET_LINK_LIBRARIES(test-network-local elle
					 ${QT_QTNETWORK_LIBRARIES}
					 ${QT_QTCORE_LIBRARIES}
                                         ${Boost_LIBRARIES})
