PROJECT(8map CXX)

ADD_EXECUTABLE(8map
  Map.cc
)

TARGET_LINK_LIBRARIES(8map elle etoile crypto ssl dl
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTCORE_LIBRARIES})
