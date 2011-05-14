PROJECT(Nucleus CXX)

ADD_LIBRARY(nucleus
  Nucleus.cc

  proton/Address.cc
  proton/Block.cc
  proton/ContentHashBlock.cc
  proton/ImprintBlock.cc
  proton/Network.cc
  proton/OwnerKeyBlock.cc
  proton/PublicKeyBlock.cc

  neutron/Access.cc
  neutron/Attributes.cc
  neutron/Author.cc
  neutron/Catalog.cc
  neutron/Data.cc
  neutron/Entry.cc
  neutron/Object.cc
  neutron/Proof.cc
  neutron/Record.cc
  neutron/Reference.cc
  neutron/Subject.cc
  neutron/Token.cc
  neutron/Trait.cc
  neutron/Voucher.cc
)

TARGET_LINK_LIBRARIES(nucleus elle)
