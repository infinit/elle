PROJECT(8Access CXX)

ADD_EXECUTABLE(8access
  Access.cc
)

TARGET_LINK_LIBRARIES(8access elle etoile crypto ssl dl
		              ${QT_QTNETWORK_LIBRARIES}
			      ${QT_QTCORE_LIBRARIES})
