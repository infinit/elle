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

#  include <cryptography/bn.hh>
#  include <cryptography/finally.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::rsa::Seed)

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::rsa::Seed,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ELLE_ASSERT_NEQ(value._n, nullptr);

  archive << value._buffer;
  archive << *value._n;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::rsa::Seed,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  ELLE_ASSERT_EQ(value._n, nullptr);

  value._n = ::BN_new();

  archive >> value._buffer;
  archive >> *value._n;
}

# endif

#endif
