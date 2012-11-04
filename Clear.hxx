#ifndef ELLE_CRYPTOGRAPHY_CLEAR_HXX
# define ELLE_CRYPTOGRAPHY_CLEAR_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Clear,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._buffer;
}

#endif
