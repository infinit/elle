PROJECT(Agent CXX)

QT4_WRAP_CPP(agent_MOCS
)

ADD_EXECUTABLE(agent
  Agent.cc

  ${agent_MOCS}
)

TARGET_LINK_LIBRARIES(agent elle crypto ssl dl
		            ${QT_QTNETWORK_LIBRARIES}
			    ${QT_QTCORE_LIBRARIES})
