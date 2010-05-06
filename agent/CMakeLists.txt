PROJECT(Agent CXX)

ADD_EXECUTABLE(Agent
  Agent.cc
)

TARGET_LINK_LIBRARIES(Agent elle lune etoile crypto ssl dl
		            ${QT_QTNETWORK_LIBRARIES}
			    ${QT_QTCORE_LIBRARIES})
