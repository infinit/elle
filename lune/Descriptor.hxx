#ifndef  LUNE_DESCRIPTOR_HXX
# define LUNE_DESCRIPTOR_HXX

# include <cassert>

# include <elle/cryptography/Signature.hh>
# include <nucleus/proton/Address.hxx>
# include <hole/Model.hh>

# include <lune/Descriptor.hh>

ELLE_SERIALIZE_SIMPLE(lune::Descriptor,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value._id;
  archive & value.name;
  archive & value.model;
  archive & value.root;
  archive & value.history;
  archive & value.extent;
  archive & value.contention;
  archive & value.balancing;
  archive & value.signature;
}

#endif
