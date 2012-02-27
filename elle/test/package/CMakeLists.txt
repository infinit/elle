ADD_EXECUTABLE(test-package EXCLUDE_FROM_ALL
  Generator.cc
  Pack.cc
  Referee.cc
  Test.cc
)

TARGET_LINK_LIBRARIES(test-package elle
				   ${QT_QTNETWORK_LIBRARIES}
				   ${QT_QTCORE_LIBRARIES})
