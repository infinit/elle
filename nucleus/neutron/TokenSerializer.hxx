#ifndef  NUCLEUS_NEUTRON_TOKENSERIALIZER_HXX
# define NUCLEUS_NEUTRON_TOKENSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/PointerSerializer.hxx>
# include <elle/cryptography/CodeSerializer.hxx>

# include <nucleus/neutron/Token.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Token,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & elle::serialize::pointer(value.code);
}

#endif
