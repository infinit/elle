#include "../cryptography.hh"

#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/pem.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
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

// One can generate this key through the following command:
//
//   $> ssh-keygen -t rsa -f output
//
// This will generate 'output' and 'output.pub' files.

elle::String private_key(
R"PRIVATEKEY(-----BEGIN DSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: AES-128-CBC,50268ADB8A0C8236CDC33EB5377899F3

qB5vL0bet1V2fTTXugBNdum1of65Y7ekLOINvMW7YaL4oehBVsvu+C9wj94YTvyC
ZTifRCJxloobo5eeXUxiILs8U4sk4IyP4Zn+ZV4tBys/SZXYV2W5XmxMDdmXLg4v
9UW98ROkc+d7jtGnt6EMGW+sZciVCb4oA2jsiv2sjtaqE4OqgnQGv/FVK5j/Z/y1
JjDcLpssihntJF4NyqwZgKzC33IdWLWTGc3lOAXSbNiGmQg/xXqwq5lo4NlCOKL+
GS8qtqdYJKAl7F2RX3OsoO0qibYIqylDIy1Bew4YBOa5GqZdJbVN2fXKZ0vfFbMp
uxGTizqGt/J0k3RTdS1/7AN6Knr4fyZqzhOM/of20bb9R3GQexp1gslAioAzHdtT
uFwLZewRBYfBetoG+Cw6mWe69lm++hwM4NWpRc2bOE0YY+PCBuwYt07Fcq07nC3a
v8KZrajqaN3bcqAgRHlC5WF6yjoc3Furv5zOLkBw2/RnoccxAUv/j7yGRGJOMwPs
naf9saE5CSZIb/FyKmDedKAtdnVNf28/Dl5SkTL6DfBTOPEMAKMRtJJ5TJS4o7fk
PctppG3uCJqoSLPWSLNNIA==
-----END DSA PRIVATE KEY-----)PRIVATEKEY");

static
void
test_operate_import()
{
  elle::String const passphrase = "Sancho";

  // 1) Try to load the DSA private key with an incorrect
  // passphrase 2) Load DSA private key in its encrypted form.
  boost::filesystem::path path_private_key =
    _temporary(private_key);

  // 1)
  BOOST_CHECK_THROW(
    infinit::cryptography::dsa::pem::import_k(path_private_key,
                                              "wrong passphrase"),
    infinit::cryptography::Exception);

  // 2)
  infinit::cryptography::dsa::PrivateKey k =
    infinit::cryptography::dsa::pem::import_k(path_private_key,
                                              passphrase);
  infinit::cryptography::dsa::PublicKey K(k);

  boost::filesystem::remove(path_private_key);

  // Sign and verify data to make sure the keys are valid.
  elle::String const data("N'est pas Sancho qui veut!");

  elle::Buffer signature = k.sign(data);
  BOOST_CHECK_EQUAL(K.verify(signature, data), true);
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
  infinit::cryptography::dsa::PublicKey K(k);

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
