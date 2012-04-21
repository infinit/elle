#ifndef  LUNE_AUTHORITYSERIALIZER_HXX
# define LUNE_AUTHORITYSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/PointerSerializer.hxx>
# include <elle/cryptography/CipherSerializer.hxx>

# include <lune/Authority.hh>

ELLE_SERIALIZE_SIMPLE(lune::Authority,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.type;
  archive & elle::serialize::alive_pointer(value.cipher);
}

#endif
