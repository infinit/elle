PROJECT(Agent CXX)

ADD_EXECUTABLE(Agent
  Agent.cc
  agent.cc
)

TARGET_LINK_LIBRARIES(Agent infinit elle nucleus lune
		            ${QT_QTCORE_LIBRARIES}
		            ${QT_QTNETWORK_LIBRARIES}
			    ${QT_QTCORE_LIBRARIES})
