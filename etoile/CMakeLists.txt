PROJECT(Core CXX)

ADD_EXECUTABLE(infinit
  main.cc
  components/Directory.cc
  components/Access.cc
  components/Address.cc
  components/File.cc
  components/Entity.cc
  components/Block.cc
  components/PublicKeyBlock.cc
  components/ContentHashBlock.cc
  )

TARGET_LINK_LIBRARIES(infinit elle crypto ssl dl)
