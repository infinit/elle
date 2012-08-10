
add_definitions(-fPIC)

add_library(hole
  implementations/local/Implementation.cc
  implementations/local/Local.cc
  implementations/local/Machine.cc

  implementations/remote/Client.cc
  implementations/remote/Customer.cc
  implementations/remote/Implementation.cc
  implementations/remote/Machine.cc
  implementations/remote/Remote.cc
  implementations/remote/Server.cc

  implementations/slug/Bank.cc
  implementations/slug/Cluster.cc
  implementations/slug/Guestlist.cc
  implementations/slug/Host.cc
  implementations/slug/Implementation.cc
  implementations/slug/Machine.cc
  implementations/slug/Neighbourhood.cc
  implementations/slug/Slug.cc

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

target_link_libraries(hole elle protocol infinit nucleus lune hole agent metal ${Boost_LIBRARIES})

add_executable(8hole _hole.cc)
target_link_libraries(8hole hole)
