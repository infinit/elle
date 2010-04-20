PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  configuration/Configuration.cc

  kernel/Kernel.cc
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
  kernel/Access.cc
  kernel/Attributes.cc
  kernel/Entry.cc
  kernel/Record.cc
  kernel/Trait.cc
  kernel/Subject.cc
  kernel/Author.cc

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

  context/Context.cc
  context/Object.cc
  context/Directory.cc
  context/File.cc
  context/Link.cc
  context/Rights.cc
  context/Identifier.cc

  components/Object.cc
  components/Directory.cc
  components/File.cc
  components/Link.cc
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
  wall/File.cc
  wall/Link.cc
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
