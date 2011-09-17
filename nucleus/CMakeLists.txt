PROJECT(Nucleus CXX)

ADD_LIBRARY(nucleus
  Nucleus.cc

  proton/Address.cc
  proton/Base.cc
  proton/Block.cc
  proton/ContentHashBlock.cc
  proton/History.cc
  proton/ImmutableBlock.cc
  proton/ImprintBlock.cc
  proton/Location.cc
  proton/MutableBlock.cc
  proton/Network.cc
  proton/Nodule.cc
  proton/OwnerKeyBlock.cc
  proton/Proton.cc
  proton/PublicKeyBlock.cc
  proton/Stamp.cc
  proton/Version.cc

  neutron/Access.cc
  neutron/Attributes.cc
  neutron/Author.cc
  neutron/Catalog.cc
  neutron/Data.cc
  neutron/Entry.cc
  neutron/Object.cc
  neutron/Record.cc
  neutron/Reference.cc
  neutron/Subject.cc
  neutron/Token.cc
  neutron/Trait.cc
)

TARGET_LINK_LIBRARIES(nucleus elle)
