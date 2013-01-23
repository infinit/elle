#ifndef INFINIT_CRYPTOGRAPHY_INPUT_HXX
# define INFINIT_CRYPTOGRAPHY_INPUT_HXX

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Input,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  archive & value._buffer;
}

#endif
