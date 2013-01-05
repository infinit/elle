project(Nucleus CXX)

add_library(nucleus ${LINK_METHOD}
  factory.cc
  Derivable.cc

  proton/Address.cc
  proton/Base.cc
  proton/Block.cc
  proton/Clef.cc
  proton/ContentHashBlock.cc
  proton/Contents.cc
  proton/Egg.cc
  proton/Family.cc
  proton/Flags.cc
  proton/Handle.cc
  proton/History.cc
  proton/ImmutableBlock.cc
  proton/ImprintBlock.cc
  proton/Limits.cc
  proton/Location.cc
  proton/Mode.hh
  proton/MutableBlock.cc
  proton/Nature.cc
  proton/Nest.cc
  proton/Network.cc
  proton/Node.cc
  proton/OwnerKeyBlock.cc
  proton/PublicKeyBlock.cc
  proton/Radix.cc
  proton/Revision.cc
  proton/Root.cc
  proton/Stamp.cc
  proton/State.cc
  proton/Statistics.cc
  proton/Strategy.cc
  proton/Value.cc

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
