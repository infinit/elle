PROJECT(Etoile CXX)

QT4_WRAP_CPP(etoile_MOCS
  depot/Record.hh
)

ADD_LIBRARY(etoile
  core/Access.cc
  core/Object.cc
  core/PublicKeyBlock.cc
  core/ContentHashBlock.cc
  core/Token.cc
  core/Proof.cc
  core/Voucher.cc
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
  depot/Record.cc

  path/Path.cc
  path/Cache.cc
  path/Item.cc
  path/Route.cc
  path/Venue.cc

  configuration/Configuration.cc

  context/Object.cc
  context/Directory.cc

  components/Object.cc
  components/Directory.cc
  components/Catalog.cc
  components/Author.cc

  agent/Agent.cc

  ${etoile_MOCS}
)

ADD_EXECUTABLE(test
  main.cc
)

TARGET_LINK_LIBRARIES(etoile elle crypto ssl dl)

TARGET_LINK_LIBRARIES(test elle crypto ssl dl etoile
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTCORE_LIBRARIES})
