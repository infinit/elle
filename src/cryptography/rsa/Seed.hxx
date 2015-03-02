#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# ifndef INFINIT_CRYPTOGRAPHY_RSA_SEED_HXX
#  define INFINIT_CRYPTOGRAPHY_RSA_SEED_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

#  include <elle/serialize/Serializer.hh>

#  include <cryptography/finally.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::Seed)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::Seed,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  archive << value._buffer;
  archive << value._length;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::Seed,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  archive >> value._buffer;
  archive >> value._length;
}

# endif

#endif
