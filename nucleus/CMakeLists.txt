project(Nucleus CXX)

add_library(nucleus ${LINK_METHOD}
  Nucleus.cc
  Derivable.cc

  proton/Action.cc
  proton/Address.cc
  proton/Base.cc
  proton/Block.cc
  proton/ContentHashBlock.cc
  proton/Family.cc
  proton/History.cc
  proton/ImmutableBlock.cc
  proton/ImprintBlock.cc
  proton/Location.cc
  proton/MutableBlock.cc
  proton/Network.cc
  proton/OwnerKeyBlock.cc
# XXX  proton/Porcupine.cc
  proton/PublicKeyBlock.cc
  proton/Stamp.cc
  proton/Transcript.cc
  proton/Revision.cc

  neutron/Access.cc
  neutron/Attributes.cc
  neutron/Author.cc
  neutron/Catalog.cc
  neutron/Component.cc
  neutron/Data.cc
  neutron/Ensemble.cc
  neutron/Entry.cc
  neutron/Fellow.cc
  neutron/Genre.cc
  neutron/Group.cc
  neutron/Object.cc
  neutron/Permissions.cc
  neutron/Record.cc
  neutron/Reference.cc
  neutron/Subject.cc
  neutron/Token.cc
  neutron/Trait.cc
)

