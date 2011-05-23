PROJECT(8shell CXX)

ADD_EXECUTABLE(8shell
  Shell.cc
)

TARGET_LINK_LIBRARIES(8shell infinit elle lune etoile readline
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
