#ifndef ELLE_CRYPTOGRAPHY_OUTPUT_HXX
# define ELLE_CRYPTOGRAPHY_OUTPUT_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Output,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._buffer;
}

#endif
