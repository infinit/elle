PROJECT(8diary CXX)

ADD_EXECUTABLE(8diary
  Diary.cc
  Memoirs.cc

  unix/Crux.cc
  unix/Live.cc
  unix/Memoirs.cc
  unix/Record.cc
  unix/Replay.cc
  unix/Upcall.cc
)

TARGET_LINK_LIBRARIES(8diary infinit elle nucleus lune etoile facade
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
