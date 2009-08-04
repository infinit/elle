PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  core/Directory.cc
  core/Access.cc
  core/Address.cc
  core/File.cc
  core/Block.cc
  core/Object.cc
  core/PublicKeyBlock.cc
  core/ContentHashBlock.cc
  core/Token.cc
  core/Proof.cc
  core/Voucher.cc
  core/Catalog.cc
  core/References.cc
  core/Data.cc
  core/Link.cc

  cache/Cache.cc
  )

ADD_EXECUTABLE(test
  main.cc
  )

TARGET_LINK_LIBRARIES(etoile elle crypto ssl dl)

TARGET_LINK_LIBRARIES(test elle crypto ssl dl etoile)
