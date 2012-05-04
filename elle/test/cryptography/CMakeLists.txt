ADD_EXECUTABLE(test-cryptography EXCLUDE_FROM_ALL
  Test.cc
)

TARGET_LINK_LIBRARIES(test-cryptography elle comet
					${QT_QTNETWORK_LIBRARIES}
					${QT_QTCORE_LIBRARIES}
                                        ${Boost_LIBRARIES})
