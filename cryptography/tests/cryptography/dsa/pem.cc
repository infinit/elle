#include "../cryptography.hh"

#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/Padding.hh>
#include <cryptography/dsa/pem.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>
#include <elle/Exception.hh>

#include <cstdio>
#include <fstream>

#include <unistd.h>

/*----------.
| Utilities |
`----------*/

static
boost::filesystem::path
_temporary(elle::String const& content = "")
{
  char path[L_tmpnam];
  strcpy(path, "/tmp/infinit-cryptography-dsa-pem-XXXXX");

  if (::mkstemp(path) == -1)
    throw elle::Exception(
      elle::sprintf("unable to create a temporary file: %s",
                    std::error_code(errno,
                                    std::system_category()).message()));

  std::ofstream stream(path, std::ofstream::out);
  stream << content;
  stream.close();

  return (boost::filesystem::path(path));
}

/*--------.
| Operate |
`--------*/

// XXX explain how to generate those

static elle::String const private_key_encrypted("-----BEGIN DSA PRIVATE KEY-----\n\
XXX\n\
-----END DSA PRIVATE KEY-----");

static elle::String const private_key_unencrypted("-----BEGIN DSA PRIVATE KEY-----\n\
XXX\n\
-----END DSA PRIVATE KEY-----");

static elle::String const public_key("-----BEGIN PUBLIC KEY-----\n\
XXX\n\
-----END PUBLIC KEY-----");

static
void
test_operate_import()
{
  elle::String const passphrase = "Sancho";

  // Load DSA public key.
  boost::filesystem::path path_public_key =
    _temporary(public_key);

  infinit::cryptography::dsa::PublicKey K =
    infinit::cryptography::dsa::pem::import_K(path_public_key);

  boost::filesystem::remove(path_public_key);

  // 1) Try to load the DSA private key with an incorrect
  // passphrase 2) Load DSA private key in its encrypted form.
  boost::filesystem::path path_private_key_encrypted =
    _temporary(private_key_encrypted);

  // 1)
  BOOST_CHECK_THROW(
    infinit::cryptography::dsa::pem::import_k(path_private_key_encrypted,
                                              "wrong passphrase"),
    infinit::cryptography::Exception);

  // 2)
  infinit::cryptography::dsa::PrivateKey k =
    infinit::cryptography::dsa::pem::import_k(path_private_key_encrypted,
                                              passphrase);

  boost::filesystem::remove(path_private_key_encrypted);

  // Load DSA keypair through the private key in its unencrypted form.
  boost::filesystem::path path_private_key_unencrypted =
    _temporary(private_key_unencrypted);

  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::pem::import_keypair(
      path_private_key_unencrypted,
      "");

  boost::filesystem::remove(path_private_key_encrypted);

  BOOST_CHECK_EQUAL(K, keypair.K());
  BOOST_CHECK_EQUAL(k, keypair.k());

  // Encrypt and decrypt data to make sure the keys are valid.
  elle::String const data("N'est pas Sancho qui veut!");

  infinit::cryptography::Code code = K.encrypt(data);
  elle::String _data = k.decrypt<elle::String>(code);

  BOOST_CHECK_EQUAL(data, _data);
}

static
void
test_operate_export()
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(2048);

  boost::filesystem::path path = _temporary();

  elle::String const passphrase = "Dave";

  // Export keypair.
  infinit::cryptography::dsa::pem::export_keypair(
    keypair,
    path,
    passphrase,
    infinit::cryptography::Cipher::aes256,
    infinit::cryptography::Mode::cbc);

  // 1) Try to re-import with wrong passphrase 3) Re-import private key.

  // 1)
  BOOST_CHECK_THROW(
    infinit::cryptography::dsa::pem::import_k(path,
                                              "wrong passphrase"),
    infinit::cryptography::Exception);

  // 2)
  infinit::cryptography::dsa::PrivateKey k =
    infinit::cryptography::dsa::pem::import_k(path,
                                              passphrase);

  BOOST_CHECK_EQUAL(keypair.k(), k);

  // Try to import only the public part of the key from a private key file
  // which is encrypted.
  BOOST_CHECK_THROW(
    infinit::cryptography::dsa::pem::import_K(path),
    infinit::cryptography::Exception);

  // Extract the public key from the private key.
  infinit::cryptography::dsa::PublicKey K(k,
                                          infinit::cryptography::Cipher::des,
                                          infinit::cryptography::Mode::cbc);

  BOOST_CHECK_EQUAL(keypair.K(), K);
}

static
void
test_operate()
{
  test_operate_import();
  test_operate_export();
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dsa/pem");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
  suite->add(BOOST_TEST_CASE(test_operate));
#endif

  boost::unit_test::framework::master_test_suite().add(suite);
}
