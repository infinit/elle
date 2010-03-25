PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  configuration/Configuration.cc

  core/Core.cc
  core/Access.cc
  core/Object.cc
  core/PublicKeyBlock.cc
  core/ContentHashBlock.cc
  core/Token.cc
  core/Proof.cc
  core/Voucher.cc
  core/Contents.cc
  core/Catalog.cc
  core/Reference.cc
  core/Data.cc
  core/Subject.cc
  core/Author.cc
  core/Time.cc

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

  context/Object.cc
  context/Directory.cc
  context/Rights.cc

  components/Object.cc
  components/Directory.cc
  components/Catalog.cc
  components/Access.cc
  components/Rights.cc
  components/Author.cc

  journal/Journal.cc

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
)

ADD_EXECUTABLE(Etoile
  Etoile.cc
)

TARGET_LINK_LIBRARIES(etoile elle)

TARGET_LINK_LIBRARIES(Etoile elle etoile crypto ssl dl
		             ${QT_QTNETWORK_LIBRARIES}
			     ${QT_QTCORE_LIBRARIES})
