PROJECT(Hole CXX)

ADD_EXECUTABLE(Hole
  local/Local.cc

  remote/Client.cc
  remote/Node.cc
  remote/Remote.cc
  remote/Server.cc

  Hole.cc
  Holeable.cc

  hole.cc
)

TARGET_LINK_LIBRARIES(Hole elle nucleus lune
		           ${QT_QTCORE_LIBRARIES}
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTKERNEL_LIBRARIES})
