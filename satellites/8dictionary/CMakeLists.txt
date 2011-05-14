PROJECT(8dictionary CXX)

ADD_EXECUTABLE(8dictionary
  Dictionary.cc
)

TARGET_LINK_LIBRARIES(8dictionary elle etoile
		                  ${QT_QTNETWORK_LIBRARIES}
			          ${QT_QTCORE_LIBRARIES})
