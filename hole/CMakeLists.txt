PROJECT(Hole CXX)

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

ADD_EXECUTABLE(Hole
  hole.cc
)

TARGET_LINK_LIBRARIES(Hole infinit elle nucleus lune hole
		           ${QT_QTCORE_LIBRARIES}
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTKERNEL_LIBRARIES})
