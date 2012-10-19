add_library(hole ${LINK_METHOD}
  implementations/local/Implementation.cc
  implementations/local/Local.cc
  implementations/local/Machine.cc

  implementations/remote/Client.cc
  implementations/remote/Customer.cc
  implementations/remote/Implementation.cc
  implementations/remote/Machine.cc
  implementations/remote/Manifest.cc
  implementations/remote/Remote.cc
  implementations/remote/Server.cc

  implementations/slug/Host.cc
  implementations/slug/Implementation.cc
  implementations/slug/Machine.cc
  implementations/slug/Manifest.cc

  storage/Directory.cc
  storage/Storage.cc

# XXX
#  implementations/cirkle/Cirkle.cc
#  implementations/cirkle/Cluster.cc
#  implementations/cirkle/Implementation.cc
#  implementations/cirkle/Machine.cc
#  implementations/cirkle/Neighbour.cc
#  implementations/cirkle/Neighbourhood.cc
#  implementations/cirkle/RoutingTable.cc

  Hole.cc
  Label.cc
  Model.cc
  Openness.cc
)

#add_executable(8hole _hole.cc)
#target_link_libraries(8hole hole)
