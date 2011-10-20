ADD_LIBRARY(hole
  implementations/local/Implementation.cc
  implementations/local/Local.cc
  implementations/local/Machine.cc

  implementations/remote/Client.cc
  implementations/remote/Customer.cc
  implementations/remote/Implementation.cc
  implementations/remote/Machine.cc
  implementations/remote/Remote.cc
  implementations/remote/Server.cc

  implementations/slug/Slug.cc
  implementations/slug/Cluster.cc
  implementations/slug/Implementation.cc
  implementations/slug/Machine.cc
  implementations/slug/Neighbour.cc
  implementations/slug/Neighbourhood.cc
  implementations/slug/RoutingTable.cc
 
# XXX
#  implementations/cirkle/Cirkle.cc
#  implementations/cirkle/Cluster.cc
#  implementations/cirkle/Implementation.cc
#  implementations/cirkle/Machine.cc
#  implementations/cirkle/Neighbour.cc
#  implementations/cirkle/Neighbourhood.cc
#  implementations/cirkle/RoutingTable.cc

  Hole.cc
  Holeable.cc
  Label.cc
  Model.cc
)

TARGET_LINK_LIBRARIES(hole elle)

ADD_EXECUTABLE(Hole
  _hole.cc
)

TARGET_LINK_LIBRARIES(Hole infinit nucleus lune hole
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTCORE_LIBRARIES})
