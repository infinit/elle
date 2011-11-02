PROJECT(8access CXX)

ADD_EXECUTABLE(8access
  Access.cc
)

TARGET_LINK_LIBRARIES(8access infinit elle lune etoile agent hole
		              ${QT_QTNETWORK_LIBRARIES}
			      ${QT_QTCORE_LIBRARIES})
