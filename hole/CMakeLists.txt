PROJECT(Hole CXX)

ADD_LIBRARY(hole
  implementations/local/Local.cc

  implementations/remote/Client.cc
  implementations/remote/Customer.cc
  implementations/remote/Peer.cc
  implementations/remote/Remote.cc
  implementations/remote/Server.cc

  implementations/cirkle/Cirkle.cc
  implementations/cirkle/Cluster.cc
  implementations/cirkle/Neighbour.cc
  implementations/cirkle/Neighbourhood.cc
  implementations/cirkle/Peer.cc
  implementations/cirkle/RoutingTable.cc

  Hole.cc
  Holeable.cc
  Label.cc
  Model.cc
)

ADD_EXECUTABLE(Hole
  hole.cc
)

TARGET_LINK_LIBRARIES(Hole infinit elle nucleus lune hole
		           ${QT_QTCORE_LIBRARIES}
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTKERNEL_LIBRARIES})
