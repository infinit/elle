#ifndef INFINIT_CRYPTOGRAPHY_INPUT_HXX
# define INFINIT_CRYPTOGRAPHY_INPUT_HXX

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Input,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._buffer;
}

#endif
