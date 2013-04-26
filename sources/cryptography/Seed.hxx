#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# ifndef INFINIT_CRYPTOGRAPHY_SEED_HXX
#  define INFINIT_CRYPTOGRAPHY_SEED_HXX

//
// ---------- Class -----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

#  include <elle/serialize/Serializer.hh>
#  include <elle/utility/Factory.hh>

#  include <cryptography/Seed.hh>
#  include <cryptography/Cryptosystem.hh>

ELLE_SERIALIZE_SPLIT(infinit::cryptography::Seed);

ELLE_SERIALIZE_SPLIT_SAVE(infinit::cryptography::Seed,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  archive << value._implementation->cryptosystem();

  // XXX[alive_pointer?]
  archive << *value._implementation;
}

ELLE_SERIALIZE_SPLIT_LOAD(infinit::cryptography::Seed,
                          archive,
                          value,
                          format)
{
  enforce(format == 0);

  infinit::cryptography::Cryptosystem cryptosystem;
  archive >> cryptosystem;

  ELLE_ASSERT_EQ(value._implementation, nullptr);
  value._implementation.reset(
    infinit::cryptography::seed::factory().allocate<
      infinit::cryptography::seed::Interface>(cryptosystem));

  // XXX[improve by passing the archive to the factory]
  archive >> *value._implementation;
}

//
// ---------- Interface -------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::seed::Interface,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  // XXX[nothing to do: we should not have to write this]
  (void)archive;
  (void)value;
}

# endif

#endif
