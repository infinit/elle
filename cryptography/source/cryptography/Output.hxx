#ifndef INFINIT_CRYPTOGRAPHY_OUTPUT_HXX
# define INFINIT_CRYPTOGRAPHY_OUTPUT_HXX

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Output,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._buffer;
}

#endif
