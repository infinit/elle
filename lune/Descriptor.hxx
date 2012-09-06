#ifndef  LUNE_DESCRIPTOR_HXX
# define LUNE_DESCRIPTOR_HXX

# include <cassert>

# include <elle/cryptography/Signature.hh>
# include <nucleus/proton/Address.hxx>
# include <hole/Model.hh>

ELLE_SERIALIZE_SIMPLE(lune::Descriptor,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._id;
  archive & value.name;
  archive & value.model;
  archive & value._openness;
  archive & value.root;
  archive & value._everybody.identity;
  archive & value.history;
  archive & value.extent;
  archive & value.contention;
  archive & value.balancing;
  archive & value._policy;
  archive & value.signature;
}

#endif
