#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# include "cryptography.hh"

# include <cryptography/Seed.hh>
# include <cryptography/KeyPair.hh>
# include <cryptography/SecretKey.hh>
# include <cryptography/Exception.hh>
# include <cryptography/random.hh>

# include <elle/serialize/insert.hh>
# include <elle/serialize/extract.hh>

/*--------.
| Helpers |
`--------*/

static
infinit::cryptography::KeyPair
_test_generate_rsa(elle::Natural32 const length)
{
  infinit::cryptography::KeyPair keypair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      length);

  return (keypair);
}

/*----------.
| Represent |
`----------*/

static
void
test_represent_rsa()
{
  // 1)
  {
    infinit::cryptography::Seed seed =
      infinit::cryptography::Seed::generate(
        infinit::cryptography::Cryptosystem::rsa,
        1024);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << seed;
    elle::printf("[representation 1] %s\n", archive);
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate base64-based representations which can be used in
  // other tests.

  // RSA.
  test_represent_rsa();
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::Seed
test_generate_rsa()
{
  infinit::cryptography::Seed seed =
    infinit::cryptography::Seed::generate(
      infinit::cryptography::Cryptosystem::rsa,
      2048);

  return (seed);
}

static
void
test_generate()
{
  // RSA.
  test_generate_rsa();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  // RSA
  infinit::cryptography::Seed seed1 = test_generate_rsa();

  // Seed copy.
  infinit::cryptography::Seed seed2(seed1);

  BOOST_CHECK_EQUAL(seed1, seed2);

  // Seed move.
  infinit::cryptography::Seed seed3(std::move(seed1));

  BOOST_CHECK_EQUAL(seed2, seed3);
}

/*--------.
| Operate |
`--------*/

template <typename R, typename D>
static
void
_test_operate(R const& r,
              D const& d,
              infinit::cryptography::Cryptosystem cryptosystem,
              elle::Natural32 length)
{
  infinit::cryptography::Seed seed0 =
    infinit::cryptography::Seed::generate(cryptosystem, length);

  infinit::cryptography::Seed seed1 = r.rotate(seed0);

  infinit::cryptography::KeyPair keypair1(seed1);
  infinit::cryptography::PublicKey K1(seed1);
  infinit::cryptography::PrivateKey k1(seed1);

  BOOST_CHECK_EQUAL(keypair1.K(), K1);
  BOOST_CHECK_EQUAL(keypair1.k(), k1);

  infinit::cryptography::Seed seed2 = r.rotate(seed1);

  infinit::cryptography::KeyPair keypair2(seed2);
  infinit::cryptography::PublicKey K2(seed2);
  infinit::cryptography::PrivateKey k2(seed2);

  BOOST_CHECK_EQUAL(keypair2.K(), K2);
  BOOST_CHECK_EQUAL(keypair2.k(), k2);

  infinit::cryptography::Seed seed3 = r.rotate(seed2);

  infinit::cryptography::KeyPair keypair3(seed3);
  infinit::cryptography::PublicKey K3(seed3);
  infinit::cryptography::PrivateKey k3(seed3);

  infinit::cryptography::Seed _seed2 = d.derive(seed3);

  infinit::cryptography::KeyPair _keypair2(_seed2);
  infinit::cryptography::PublicKey _K2(_seed2);
  infinit::cryptography::PrivateKey _k2(_seed2);

  BOOST_CHECK_EQUAL(seed2, _seed2);
  BOOST_CHECK_EQUAL(keypair2, _keypair2);
  BOOST_CHECK_EQUAL(_keypair2.K(), _K2);
  BOOST_CHECK_EQUAL(_keypair2.k(), _k2);

  infinit::cryptography::Seed _seed1 = d.derive(_seed2);

  infinit::cryptography::KeyPair _keypair1(_seed1);
  infinit::cryptography::PublicKey _K1(_seed1);
  infinit::cryptography::PrivateKey _k1(_seed1);

  infinit::cryptography::Seed _seed0 = d.derive(_seed1);

  BOOST_CHECK_EQUAL(seed1, _seed1);
  BOOST_CHECK_EQUAL(keypair1, _keypair1);
  BOOST_CHECK_EQUAL(_keypair1.K(), _K1);
  BOOST_CHECK_EQUAL(_keypair1.k(), _k1);

  BOOST_CHECK_EQUAL(seed0, _seed0);
}

static
void
test_operate_rsa()
{
  // 512-bit K-rotate/k-derive
  {
    elle::Natural32 const length = 512;
    infinit::cryptography::KeyPair keypair = _test_generate_rsa(length);

    _test_operate(keypair.K(), keypair.k(),
                  infinit::cryptography::Cryptosystem::rsa, length);
  }

  // 2048-bit k-rotate/K-derive
  //
  // Note that since the rotation/derivation process performs a textbook
  // RSA --- i.e raising big number to big exponents ---, the larger the
  // key, the longer it will take. Say differently, do not use keys that
  // are too large for that purpose unless you can support it.
  {
    elle::Natural32 const length = 2048;
    infinit::cryptography::KeyPair keypair = _test_generate_rsa(length);

    _test_operate(keypair.k(), keypair.K(),
                  infinit::cryptography::Cryptosystem::rsa, length);
  }

  // Wrong seed/key size.
  {
    infinit::cryptography::KeyPair keypair = _test_generate_rsa(1024);

    infinit::cryptography::Seed seed =
      infinit::cryptography::Seed::generate(
        infinit::cryptography::Cryptosystem::rsa, 512);

    BOOST_CHECK_THROW(keypair.k().rotate(seed),
                      infinit::cryptography::Exception);
  }
}

static
void
test_operate()
{
  // RSA.
  test_operate_rsa();
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize()
{
  // XXX[waiting for new serialization] + represent()
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Seed");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

#endif
