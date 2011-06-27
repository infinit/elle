PROJECT(8authority CXX)

ADD_EXECUTABLE(8authority
  Authority.cc
)

TARGET_LINK_LIBRARIES(8authority infinit elle nucleus lune etoile
		                 ${QT_QTNETWORK_LIBRARIES}
			         ${QT_QTCORE_LIBRARIES})
