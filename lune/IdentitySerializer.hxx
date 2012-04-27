#ifndef  LUNE_IDENTITYSERIALIZER_HXX
# define LUNE_IDENTITYSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/PointerSerializer.hxx>

# include <lune/Identity.hh>

ELLE_SERIALIZE_SIMPLE(lune::Identity,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & elle::serialize::pointer(value.cipher);
  if (value.cipher != nullptr)
    archive & value.pair;
  archive & value.name;
  archive & value.signature;
}

#endif
