PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  configuration/Configuration.cc

  kernel/Access.cc
  kernel/Attributes.cc
  kernel/Author.cc
  kernel/Catalog.cc
  kernel/ContentHashBlock.cc
  kernel/Contents.cc
  kernel/Data.cc
  kernel/Entry.cc
  kernel/Kernel.cc
  kernel/Object.cc
  kernel/Proof.cc
  kernel/PublicKeyBlock.cc
  kernel/Record.cc
  kernel/Reference.cc
  kernel/Subject.cc
  kernel/Token.cc
  kernel/Trait.cc
  kernel/Voucher.cc

  context/Context.cc
  context/Directory.cc
  context/File.cc
  context/Identifier.cc
  context/Link.cc
  context/Object.cc
  context/Rights.cc

  components/Access.cc
  components/Attributes.cc
  components/Author.cc
  components/Directory.cc
  components/File.cc
  components/Link.cc
  components/Object.cc
  components/Rights.cc

  path/Cache.cc
  path/Item.cc
  path/Path.cc
  path/Route.cc
  path/Venue.cc
  path/Way.cc

  user/Agent.cc
  user/Application.cc
  user/Client.cc
  user/Guest.cc
  user/Map.cc
  user/User.cc

  journal/Bucket.cc
  journal/Item.cc
  journal/Journal.cc

  depot/Cell.cc
  depot/Depot.cc
  depot/Record.cc
  depot/Repository.cc
  depot/Unit.cc

  wall/Wall.cc
  wall/Interface.cc
  wall/User.cc
  wall/Object.cc
  wall/Access.cc
  wall/Directory.cc
  wall/File.cc
  wall/Link.cc
  wall/Status.cc
  wall/Attributes.cc

  hole/Hole.cc
  hole/Address.cc
  hole/Block.cc
  hole/Universe.cc

  Etoile.cc
)

ADD_EXECUTABLE(Etoile
  etoile.cc
)

TARGET_LINK_LIBRARIES(etoile elle lune)

TARGET_LINK_LIBRARIES(Etoile elle lune etoile crypto ssl dl
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTKERNEL_LIBRARIES})
