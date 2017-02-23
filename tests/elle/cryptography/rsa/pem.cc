#include "../cryptography.hh"

#include <elle/cryptography/rsa/KeyPair.hh>
#include <elle/cryptography/rsa/PublicKey.hh>
#include <elle/cryptography/rsa/PrivateKey.hh>
#include <elle/cryptography/rsa/Padding.hh>
#include <elle/cryptography/rsa/pem.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/random.hh>
#include <elle/cryptography/Error.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/Error.hh>
#include <elle/filesystem/TemporaryFile.hh>

#include <boost/filesystem.hpp>

#include <cstdio>
#include <fstream>

#include <unistd.h>

/*----------.
| Utilities |
`----------*/

static
void
_fill(boost::filesystem::path const& path,
      std::string const& content = "")
{
  std::ofstream stream(path.generic_string(),
                       std::ofstream::out);
  stream << content;
  stream.close();
}

/*--------.
| Operate |
`--------*/

// One can generate this key through the following command:
//
//   $> ssh-keygen -t rsa -f output
//
// This will generate 'output' and 'output.pub' files.

std::string private_key(
R"PRIVATEKEY(-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: AES-128-CBC,BB839C89C20C17C5F7089433A3AB728C

0O8JC+Y2hnAeS//NJGBKFdGVdTF34oS4J/nmSQ3JGRXmYQDx5Ixj0rqJRLNH9I45
j0H9bGBycrtas6KbgpK5d4pwpy2wFnL5W9gSFAuyt5f/IMpvhtxo1/S+CnZPTEEU
sfffhGnzkWNy7myyijCbIqoKTWh9AEtG+m8fayaFpgmQ0HqzXUkuPYE78PZbVg5b
Wb+cq0hrexIbQove2NbzYN0iTzr5DRnn6UWkE4ed45bynMMV+LzNaZAZ1CQppQQB
ib1mbmBPVCOSAz8TUKrH/PN4OwIvxyO6VlXf4Y8uXjy3xDDDih1w/68SUwv9v37O
U20b27Pi31Ezv+SZmRhEvy6hq36MedbdbeCeYTYMVsZC/ePpx8XBtvRlhJkkLGlS
cb2JtEd6Emch30ib9gtZkc/ywyNCNLZu/B6Mlc3VY6K4qWaxZBZu+pdUrw1os2xE
1+yVLyF8FOSI14AN4POEPiT2HiETFnpqFN0Qh8Bm2A1TKnY4VMxnqp0emskaDAM2
JB85+nh7K65WmMPwMtkjN1Q5Mh3D7DZzhtYQOUehWaon1PxCTiXhL0EiBQ2PhprC
rAsbU5B9XV6rcVDD48DiDOGuXwt4fmczh4dpzTCPRds+6H4vuPS58ekeeOt9YMrX
v3Zrs9VM7hnCFqRMS4MoqzZGcmX+I0PVuDAmYOZ5BWluxrIMxcltCZnAmOap7Ap2
UmKfXOkX3FD+eibUt/7OS+EZf+phExwOCrUtckyN82SrJbpj5UtNao7O2KHie5qA
Xmwj7dUsBh/VsXy834elU4zdA+5GdNKbArEodxCJWQR7UtmV8sHGkjNGrvD5xocf
owLlcKA726NBAQrX9eCrZ6NYYW5VenFN0O5UzFCZkziipMZOx5fBL7ixQIn+sQFF
17E1l8r4y02GgzD+bmnhPemF+6aftaI3Lqg9cpQotd7SvRBEuYdkpYxpXnjJO5Ik
VfoeiIadUVj0gUYUBsJtUN+84QhjJ14kP16FBGltT8qKp8DotP8dSSKebGv2znLj
yKXQayPmJ+waN1T9w34yMpFq1O14C8f+mMN3gIOJEYrlLh7hYD9hjl4R1FobsBQp
BcUFAXcJm1UT8gmObNE+mkasertpNYk+IU5+ONzF99rCdMcEWiioW9KtDJZBau5o
gbv/L64YZkGCZOUBz2GFsAjN6M3wjZ2g5TLjPJsvLa5deilX2zcwmoBLe5zn+9Zs
aoXbqn1et1UC2e91zBCIRRM2Qt89iZwKck6Ezlp5s6qFn4MTtZ/mIJsq5dK07axU
A/13vu0dOsx3FOBxlLq6RhfI5Pz1799Ulaf420iVC3l/SbpdX8zYaVI1opvZuasA
ATnBnK7/+rGj7h3BEvxrGubDz7CiLR1x8EpRF3pZfGN6itHH8zS79qpKfBcu6TlP
d9p2wSK59YkJZTtmOKaBb+UxLxz/vE9TpdkYix+b4Jc5kzxncM3ONoG/pSfT9IaY
QXlBOuwTPq2wfTW2gw3qmqhd8WxNYISLmlKf4UzQshI0inIDeJ8Q2CFWjLpEwL8z
wNZmGhEhd9KiAbd0nY6M5dHgnXHZSwfY2fnePQ2v4UxsFCLgKNJ0fzSp+uktrY2u
-----END RSA PRIVATE KEY-----)PRIVATEKEY");

static
void
test_operate_import()
{
  std::string const passphrase = "Sancho";

  // 1) Try to load the RSA private key with an incorrect
  // passphrase 2) Load RSA private key in its encrypted form.
  elle::filesystem::TemporaryFile path_private_key("path_private_key");
  _fill(path_private_key.path(), private_key);

  // 1)
  BOOST_CHECK_THROW(
    elle::cryptography::rsa::pem::import_k(path_private_key.path(),
                                              "wrong passphrase"),
    elle::cryptography::Error);

  // 2)
  elle::cryptography::rsa::PrivateKey k =
    elle::cryptography::rsa::pem::import_k(path_private_key.path(),
                                              passphrase);
  elle::cryptography::rsa::PublicKey K(k);

  // Encrypt and decrypt data to make sure the keys are valid.
  std::string const data("N'est pas Sancho qui veut!");

  elle::Buffer code = K.seal(data);
  elle::Buffer plain = k.open(code);

  BOOST_CHECK_EQUAL(data, plain.string());
}

static
void
test_operate_export()
{
  elle::cryptography::rsa::KeyPair keypair =
    elle::cryptography::rsa::keypair::generate(2048);

  elle::filesystem::TemporaryFile path("path");

  std::string const passphrase = "Dave";

  // Export keypair.
  elle::cryptography::rsa::pem::export_keypair(
    keypair,
    path.path(),
    passphrase,
    elle::cryptography::Cipher::aes256,
    elle::cryptography::Mode::cbc);

  // 1) Try to re-import with wrong passphrase 3) Re-import private key.

  // 1)
  BOOST_CHECK_THROW(
    elle::cryptography::rsa::pem::import_k(path.path(),
                                              "wrong passphrase"),
    elle::cryptography::Error);

  // 2)
  elle::cryptography::rsa::PrivateKey k =
    elle::cryptography::rsa::pem::import_k(path.path(),
                                              passphrase);

  BOOST_CHECK_EQUAL(keypair.k(), k);

  // Try to import only the public part of the key from a private key file
  // which is encrypted.
  BOOST_CHECK_THROW(
    elle::cryptography::rsa::pem::import_K(path.path()),
    elle::cryptography::Error);

  // Extract the public key from the private key.
  elle::cryptography::rsa::PublicKey K(k);

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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/pem");

  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);
}
