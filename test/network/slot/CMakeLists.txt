QT4_WRAP_CPP(test-network-slot_MOCS
  Node.hh
)

ADD_EXECUTABLE(test-network-slot
  Test.cc
  Probe.cc
  Node.cc

  ${test-network-slot_MOCS}
)

TARGET_LINK_LIBRARIES(test-network-slot elle
                      ${QT_QTNETWORK_LIBRARIES}
                      ${QT_QTCORE_LIBRARIES})
