#ifndef  NUCLEUS_NEUTRON_TOKEN_HXX
# define NUCLEUS_NEUTRON_TOKEN_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>

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
