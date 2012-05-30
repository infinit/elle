#ifndef  LUNE_DESCRIPTORSERIALIZER_HXX
# define LUNE_DESCRIPTORSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/SignatureSerializer.hxx>
# include <nucleus/proton/Address.hxx>
# include <hole/ModelSerializer.hxx>

# include <lune/Descriptor.hh>

ELLE_SERIALIZE_SIMPLE(lune::Descriptor,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

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
