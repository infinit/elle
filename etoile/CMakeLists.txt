PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  components/Directory.cc
  components/Access.cc
  components/Address.cc
  components/File.cc
  components/Block.cc
  components/Object.cc
  components/PublicKeyBlock.cc
  components/ContentHashBlock.cc
  components/Token.cc
  components/Proof.cc
  components/Voucher.cc
  components/Catalog.cc
  components/References.cc
  )

ADD_EXECUTABLE(test
  main.cc
  )

TARGET_LINK_LIBRARIES(etoile elle crypto ssl dl)

TARGET_LINK_LIBRARIES(test elle crypto ssl dl etoile)
