PROJECT(8debug CXX)

ADD_EXECUTABLE(8debug
  Debug.cc
)

TARGET_LINK_LIBRARIES(8debug elle etoile crypto ssl dl
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
