PROJECT(8diary CXX)

ADD_EXECUTABLE(8diary
  Diary.cc
  Memoirs.cc
)

IF(${UNIX})
  ADD_EXECUTABLE(8diary
    unix/Crux.cc
    unix/Live.cc
    unix/Memoirs.cc
    unix/Record.cc
    unix/Replay.cc
    unix/Upcall.cc
  )
ENDIF()

TARGET_LINK_LIBRARIES(8diary infinit elle nucleus lune etoile horizon
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
