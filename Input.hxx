#ifndef ELLE_CRYPTOGRAPHY_INPUT_HXX
# define ELLE_CRYPTOGRAPHY_INPUT_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Input,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._buffer;
}

#endif
