PROJECT(8diary CXX)

ADD_EXECUTABLE(8diary
  Diary.cc
  Crux.cc
)

TARGET_LINK_LIBRARIES(8diary infinit elle nucleus lune etoile pig fuse
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
