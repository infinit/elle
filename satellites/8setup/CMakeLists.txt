PROJECT(8setup CXX)

ADD_EXECUTABLE(8setup
  Setup.cc
)

TARGET_LINK_LIBRARIES(8setup infinit elle etoile lune etoile
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
