#ifndef INFINIT_CRYPTOGRAPHY_SEED_HXX
# define INFINIT_CRYPTOGRAPHY_SEED_HXX

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Seed,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  archive & value._buffer;
}

#endif
