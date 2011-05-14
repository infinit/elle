ADD_EXECUTABLE(test-cryptography
  Test.cc
)

TARGET_LINK_LIBRARIES(test-cryptography elle comet
					${QT_QTNETWORK_LIBRARIES}
					${QT_QTCORE_LIBRARIES})
