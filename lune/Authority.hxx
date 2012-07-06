#ifndef  LUNE_AUTHORITY_HXX
# define LUNE_AUTHORITY_HXX

# include <cassert>
# include <iostream>

# include <elle/serialize/Pointer.hh>

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
