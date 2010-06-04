PROJECT(8universe CXX)

ADD_EXECUTABLE(8universe
  Universe.cc
)

TARGET_LINK_LIBRARIES(8universe infinit elle lune etoile crypto ssl dl
		                ${QT_QTNETWORK_LIBRARIES}
			        ${QT_QTCORE_LIBRARIES})
