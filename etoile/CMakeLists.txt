PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  core/Access.cc
  core/Address.cc
  core/Object.cc
  core/PublicKeyBlock.cc
  core/ContentHashBlock.cc
  core/Token.cc
  core/Proof.cc
  core/Voucher.cc
  core/Catalog.cc
  core/References.cc
  core/Data.cc
  core/Contents.cc

  hole/Hole.cc

  path/Path.cc
  path/Cache.cc
  path/Item.cc
  path/Route.cc
  path/Venue.cc

  components/Directory.cc

  configuration/Configuration.cc
  )

ADD_EXECUTABLE(test
  main.cc
  )

TARGET_LINK_LIBRARIES(etoile elle crypto ssl dl)

TARGET_LINK_LIBRARIES(test elle crypto ssl dl etoile)
