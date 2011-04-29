PROJECT(8network CXX)

ADD_EXECUTABLE(8network
  Network.cc
)

TARGET_LINK_LIBRARIES(8network infinit elle lune etoile crypto ssl dl
		               ${QT_QTNETWORK_LIBRARIES}
			       ${QT_QTCORE_LIBRARIES})
