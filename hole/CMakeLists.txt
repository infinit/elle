PROJECT(Hole CXX)

ADD_LIBRARY(hole
  local/Local.cc

# XXX
#  remote/Client.cc
#  remote/Node.cc
#  remote/Remote.cc
#  remote/Server.cc

  Hole.cc
  Holeable.cc
  Model.cc
)

ADD_EXECUTABLE(Hole
  hole.cc
)

TARGET_LINK_LIBRARIES(Hole infinit elle nucleus lune hole
		           ${QT_QTCORE_LIBRARIES}
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTKERNEL_LIBRARIES})
