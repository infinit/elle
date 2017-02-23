#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# include "../cryptography.hh"

# include <elle/cryptography/SecretKey.hh>
# include <elle/cryptography/Error.hh>
# include <elle/cryptography/random.hh>
# include <elle/cryptography/rsa/Seed.hh>
# include <elle/cryptography/rsa/KeyPair.hh>
# include <elle/cryptography/rsa/PublicKey.hh>
# include <elle/cryptography/rsa/PrivateKey.hh>

# include <elle/serialization/Serializer.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent()
{
  // 1)
  {
    elle::cryptography::rsa::Seed seed =
      elle::cryptography::rsa::seed::generate(1024);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      seed.serialize(output);
    }

    elle::printf("[representation 1] %s\n", stream.str());
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

  _test_represent();
}

/*---------.
| Generate |
`---------*/

static
elle::cryptography::rsa::Seed
_test_generate(uint32_t const length = 2048)
{
  elle::cryptography::rsa::Seed seed =
    elle::cryptography::rsa::seed::generate(length);

  return (seed);
}

static
void
test_generate()
{
  _test_generate();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  elle::cryptography::rsa::Seed seed1 = _test_generate(4096);

  // Seed copy.
  elle::cryptography::rsa::Seed seed2(seed1);

  BOOST_CHECK_EQUAL(seed1, seed2);

  // Seed move.
  elle::cryptography::rsa::Seed seed3(std::move(seed1));

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
              uint32_t length)
{
  elle::cryptography::rsa::Seed seed0 =
    elle::cryptography::rsa::seed::generate(length);

  elle::cryptography::rsa::Seed seed1 = r.rotate(seed0);

  elle::cryptography::rsa::KeyPair keypair1(seed1);
  elle::cryptography::rsa::PublicKey K1(seed1);
  elle::cryptography::rsa::PrivateKey k1(seed1);

  BOOST_CHECK_EQUAL(keypair1.K(), K1);
  BOOST_CHECK_EQUAL(keypair1.k(), k1);

  elle::cryptography::rsa::Seed seed2 = r.rotate(seed1);

  elle::cryptography::rsa::KeyPair keypair2(seed2);
  elle::cryptography::rsa::PublicKey K2(seed2);
  elle::cryptography::rsa::PrivateKey k2(seed2);

  BOOST_CHECK_EQUAL(keypair2.K(), K2);
  BOOST_CHECK_EQUAL(keypair2.k(), k2);

  elle::cryptography::rsa::Seed seed3 = r.rotate(seed2);

  elle::cryptography::rsa::KeyPair keypair3(seed3);
  elle::cryptography::rsa::PublicKey K3(seed3);
  elle::cryptography::rsa::PrivateKey k3(seed3);

  elle::cryptography::rsa::Seed _seed2 = d.unrotate(seed3);

  elle::cryptography::rsa::KeyPair _keypair2(_seed2);
  elle::cryptography::rsa::PublicKey _K2(_seed2);
  elle::cryptography::rsa::PrivateKey _k2(_seed2);

  BOOST_CHECK_EQUAL(seed2, _seed2);
  BOOST_CHECK_EQUAL(keypair2, _keypair2);
  BOOST_CHECK_EQUAL(_keypair2.K(), _K2);
  BOOST_CHECK_EQUAL(_keypair2.k(), _k2);

  elle::cryptography::rsa::Seed _seed1 = d.unrotate(_seed2);

  elle::cryptography::rsa::KeyPair _keypair1(_seed1);
  elle::cryptography::rsa::PublicKey _K1(_seed1);
  elle::cryptography::rsa::PrivateKey _k1(_seed1);

  elle::cryptography::rsa::Seed _seed0 = d.unrotate(_seed1);

  BOOST_CHECK_EQUAL(seed1, _seed1);
  BOOST_CHECK_EQUAL(keypair1, _keypair1);
  BOOST_CHECK_EQUAL(_keypair1.K(), _K1);
  BOOST_CHECK_EQUAL(_keypair1.k(), _k1);

  BOOST_CHECK_EQUAL(seed0, _seed0);
}

static
void
test_operate()
{
  // 2048-bit k-rotate/K-unrotate.
  //
  // Note that since the rotation/derivation process performs a textbook
  // RSA --- i.e raising big number to big exponents ---, the larger the
  // key, the longer it will take. Say differently, do not use keys that
  // are too large for that purpose unless you can support it.
  {
    uint32_t const length = 2048;
    elle::cryptography::rsa::KeyPair keypair =
      elle::cryptography::rsa::keypair::generate(length);

    _test_operate(keypair.k(), keypair.K(), length);
  }

  // Wrong seed/key size.
  {
    elle::cryptography::rsa::KeyPair keypair =
      elle::cryptography::rsa::keypair::generate(1024);

    elle::cryptography::rsa::Seed seed =
      elle::cryptography::rsa::seed::generate(512);

    BOOST_CHECK_THROW(keypair.k().rotate(seed),
                      elle::cryptography::Error);
  }
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize()
{
  // Serialize/deserialize.
  {
    elle::cryptography::rsa::Seed seed1 = _test_generate(2048);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      seed1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    elle::cryptography::rsa::Seed seed2(input);

    BOOST_CHECK_EQUAL(seed1, seed2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"buffer":"ACbE7eMBCuKT36AzLzoYlxtCKbUH6bMTgBhNu+3jLByV8QqqnsTTclL6El/d6dfnaqcPalMiHSzoQITIIoputmGyVIttWr5ZuP5tdHRZhq3kFcsZ4YP6+x2P5TJi+R67t50bO8nJdw3nRIbtrYHIEYhvDHYwtxBjQaYltwgbU3A=","length":1024})JSON"
    };

    elle::cryptography::test::formats<
      elle::cryptography::rsa::Seed>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/Seed");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

#endif
