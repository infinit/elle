PROJECT(Hole CXX)

ADD_LIBRARY(hole
  implementations/local/Local.cc

  implementations/remote/Client.cc
  implementations/remote/Node.cc
  implementations/remote/Remote.cc
  implementations/remote/Server.cc

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
