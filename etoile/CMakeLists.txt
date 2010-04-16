PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  configuration/Configuration.cc

  kernel/Kernel.cc
  kernel/Access.cc
  kernel/Object.cc
  kernel/PublicKeyBlock.cc
  kernel/ContentHashBlock.cc
  kernel/Token.cc
  kernel/Proof.cc
  kernel/Voucher.cc
  kernel/Contents.cc
  kernel/Catalog.cc
  kernel/Reference.cc
  kernel/Data.cc
  kernel/Subject.cc
  kernel/Author.cc
  kernel/Record.cc
  kernel/Entry.cc
  kernel/Set.cc
  kernel/Range.cc
  kernel/Attributes.cc
  kernel/Trait.cc
  kernel/Collection.cc

  hole/Hole.cc
  hole/Address.cc
  hole/Block.cc

  depot/Depot.cc
  depot/Repository.cc
  depot/Record.cc
  depot/Cell.cc
  depot/Unit.cc

  path/Path.cc
  path/Cache.cc
  path/Item.cc
  path/Route.cc
  path/Venue.cc
  path/Way.cc

  context/Object.cc
  context/Directory.cc
  context/Rights.cc
  context/Identifier.cc
  context/Context.cc

  components/Object.cc
  components/Directory.cc
  components/Access.cc
  components/Rights.cc
  components/Author.cc
  components/Attributes.cc

  journal/Journal.cc
  journal/Bucket.cc
  journal/Item.cc

  user/User.cc
  user/Client.cc
  user/Agent.cc
  user/Application.cc
  user/Guest.cc
  user/Map.cc

  wall/Wall.cc
  wall/Interface.cc
  wall/User.cc
  wall/Object.cc
  wall/Access.cc
  wall/Directory.cc
  wall/State.cc
  wall/Attributes.cc
)

ADD_EXECUTABLE(Etoile
  Etoile.cc
)

TARGET_LINK_LIBRARIES(etoile elle)

TARGET_LINK_LIBRARIES(Etoile elle etoile crypto ssl dl
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTKERNEL_LIBRARIES})
