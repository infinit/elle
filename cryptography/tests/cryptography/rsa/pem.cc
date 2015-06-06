#include "../cryptography.hh"

#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/rsa/pem.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>
#include <elle/Exception.hh>

#include <boost/filesystem.hpp>

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
  /* XXX
  char path[L_tmpnam];

  ::strncpy(path, "/tmp/infinit-cryptography-rsa-pem-XXXXX", L_tmpnam);

  if (::mkstemp(path) == -1)
    throw elle::Exception(
      elle::sprintf("unable to create a temporary file: %s",
                    std::error_code(errno,
                                    std::system_category()).message()));

  std::ofstream stream(path, std::ofstream::out);
  stream << content;
  stream.close();

  return (boost::filesystem::path(path));
  */
  boost::filesystem::path path("/");
  return path;
}

/*--------.
| Operate |
`--------*/

// XXX explain how to generate those

static elle::String const private_key_encrypted("-----BEGIN RSA PRIVATE KEY-----\n\
Proc-Type: 4,ENCRYPTED\n\
DEK-Info: DES-EDE3-CBC,2D218A3959EC071F\n\
\n\
CJOZS++AFia6Z8DV/wPhtx+LMaJZMOVDONeCS+FpRp+dEFK0FRshm8PI5VOg65+d\n\
Y8xbekOLEix70QtRMFUjDekE9fbLUzqYzyK8gIz8nyhc3BcUksM3ghcUep51FcJX\n\
a+C4bPK3rPsMtep3WGV1NM8owniOFgS+atjr9iLhJiEiB659O1idLHw1hvBL0lzt\n\
euobka+2q4LFSG/eO3YDV/JG3oOGDiZRAvUbYaI2X+o2ri0MeJJq1lAuBEnU0TiU\n\
2p3ZE0I8eZpRRfQiNrSqcpuq4qt1xD4pC2Wl38JU8yNiA3nEVrlFUaeW9qIRgqdT\n\
9ZksUjjLT8OC6wQ+SAVm3glXcR7NVXmYt8BxMmCqZNJ8LdGsUnI7fPmHcOipxZ6K\n\
n6N+VdtxsEAqp0kS6iIaEYSTvFlPUU6t/HxFH7zFCgJKmEq2TuDHuUmhLRiDJrFn\n\
N2sd8G50seQnzZbAW+g7dYWWmgg73WAhmnM2MrSATIRyo1gyr1oju5uFqoLbvTOg\n\
ZOms2PT2nlOi8SDqnR0/Dr+6/Q0gbyPjM4qeiqp3sMgn4i+Zwq9BPC/YiklQpThj\n\
p7dCK89F+XYSNpedQ6OeKWUVcbLdSffHXPcNerB28DrpYeq60yu+7G/svhrGJQq3\n\
qcEcodf5yajoVD8Yf0sCyFtJLO2rEeBVyVWixv6Wab4mHEg/f0wRBH4IUXNJw9nw\n\
Avvy6S4sS1Jt8aUMvTtOTmf1DFL/trzWDnAhZ+vNi9cDblUYlH7lmpaPLOTDZrcz\n\
E3UKfgF/6MPc8/pZD6GFUleSRHLZlfEwg331JXMZyCMcZVw50cwZwCHPmMBVkz7j\n\
T4FYfdPILIdJuo+gG142JAW5OXSlSE1Q4GEPBO/7SegFJzGg4kZGmF7rKbdAyR1y\n\
k1XYflo3lLzowlcvNAAXS2oWUnU8QZuruYB6sV1lefNEezSeQnyuufX7R9UY+c7v\n\
3zGT+2DG3mkcaQi/SMTHGuT7kRtcowlj1VNhN8vdsUfp3PlOZLzal6bRDrHVrwvG\n\
3GdTxqwmDGbU+Jqph62Xwf73YqM2M+02pDCAPBVj4nJIbKYE8HUjnh9H52Zk42+7\n\
nQb4GZqP5NJ05UU8sSEILH6KzdzHIjRuOvXLKD3m9LuzNirDE8/hlQlUd+300wVZ\n\
wdWMWai4yB4FxTXKRw/ykGqTZHtnHYYcZzRxbkdjx3MQZKN+FT3SXtiIPc1uBZr6\n\
eZjVZSRuaCCw7QA/l0TgUMi9HoaTAr/iqsMQBLjZ5gEKlHlEkHFlP8K4VDWwKVoX\n\
UzlACO5BEbG4IZVYgnlOLfbYKaMVK5cSX1gPRgyw57zIHVBoMx+MebVvN/B75MGj\n\
jeXXp7SsY+YkNVPtT7Erf/hJy2V6NzZ0lN1pI7Nc3+RWICzYkC+lxt5QU3LqL1zk\n\
x6W+mbaRqv9d0TUfqbygTPoHVm/eZWA4fOxpwGTM1fG1vhlPwEsPx/s/PnuU7YyE\n\
BWK0lHQXQNJ2SDVT9kfwzTuvIMevmXfQH+A7awoTPPB0/w6t3yV4KeTTSwXecLRI\n\
TgAyfYUkbOvUACq0Re551wLhKI48Frh9dgqt57DfKnX0eD+Gu7/5Q1V83Y0HZFCa\n\
-----END RSA PRIVATE KEY-----");

static elle::String const private_key_unencrypted("-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpQIBAAKCAQEAueF3c1cNjDEzRrRNEIMgMC1wXUqu6W8x+0GBiNVqBuvfo1RW\n\
turhyAEkBRDQ9OfqWqvVPEsE3A/aARo+sn7j9vauQe7m3INmxYKGXVDWKoNNOoSv\n\
b5C56wbYx36+8Bamxua7/ga2PiSFlhp+ajn4mknRM+YQI3SFrS0LV/Fgwb+SL8nL\n\
Mp0QRKna1Wz7l8T4Fa3SSfwe+YZDCy35EJajb6gknHVgavtEVT+zgvzcQ+FTBRoJ\n\
JCNHSygvBbaOCRBwEk4nvCgPpG415m20Toev5AVQvG2FC4CH5KMa7ntAiWvDPRWY\n\
h4VWkDQiDEEhKgDCq5AMjRgZexHD3V7IrdG3JQIDAQABAoIBAQCeFo37AJy7ENUl\n\
Ww737i2gC+U20t+0FSgTIwFCGQ/V1yIazQ37AAUKjz3NOSIeSGfeuanfX4ZAixLE\n\
QUf1k0narq5B0SdzgCYV8Q7JqoibG5TBIjv6zMHyi54u/TLzCrDJFU/pmlfAvlsG\n\
8H13ZtpyjjloxFARd3aBdH/jSZ0a6Zmafd6eZNYjQy706aT1N9naUcChmnJafUw3\n\
iSqIlPw4uB5hf0E1G9vzDb4/aOKbY6w9j6z4AuHYCx2Qv79gV0A4PsNgrSg0xYok\n\
tWhuJoYduwu9hPmvov43kOpsqbULpBcGWfUKvj64N0P0kj4xyUUgmjmWkG6Lej+X\n\
T2rGfQS5AoGBAOYm74HycFOqMBk4WSSVtEfHXJAuodWhWjaNb7sgUahFc2MiJlKi\n\
mjdGaP+OGTEvBJgUbiNjMVkMshTr1MbQmNScEolOvxQY7O5PffijG9gJw90PCz5i\n\
tWV/OPXy2Ms6lZcQAWLwY6dP1XF7jFcs/FddZl0BXMProOqUgnJHgJ1rAoGBAM7B\n\
sUQrceBFhNoOgFymF8jQHXR3b4rvqEGi94r43ESicBmzRZbVbWH6D84okqPVJeNO\n\
Yr+NHodNpB/xpFfABQn6mvzoAHP0A231KrNulEAcUJoLPc/Y0radvwMuVv0fncAH\n\
fGqNewTvThJeGbzhSaYKRffQ+9KnRj/IPGvB1hGvAoGBANm3g2J+IVxD5nQ1Jb7C\n\
D2THZvzN4DHjDSAF8+gkzppL+LeSmoYZY6Febf0yQwqP29yxlu7K6ue+Zsf7v7DD\n\
q4+UhMR3RbKRUbCuahBG3BQmSK5ZiN1fKxH0o8GmFQdLSC866NTbaqnfj55mrX/a\n\
Amvy4t6gZ0y/Hd5K1oucsEI5AoGBAI9lPNmHvCIMdRh7CEG5nDNVS6h0EXNtxedH\n\
7KGA+dvMQS1B7ZiwC33xXbbg4tsnwHBG9Cvf5vV2Psc843tMnPIP0iBnk7DX4u0H\n\
JYduwPk56hj8hPE7cuDexRSitgg2PkVpq7uGxnRkuGP6yaT7JwY2h9JZKd4uz4T7\n\
DhS6SgrXAoGABP8ZUzDl8fXhRMNq9g2I7aJDf3kNijKxJogZfxMFTLf+xQJ+vfB2\n\
uRGEU3h3r5KbCqoMpWnJIzgCzDdlfnTlusrAcUrqRvjur39Sub3znKbdYL3k6FxA\n\
nokA57DSVZTi8Ayp+QRIQ4zjd8ni8u3yNOxEvaAEaYsHWmfZVVMDVo0=\n\
-----END RSA PRIVATE KEY-----");

static elle::String const public_key("-----BEGIN PUBLIC KEY-----\n\
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAueF3c1cNjDEzRrRNEIMg\n\
MC1wXUqu6W8x+0GBiNVqBuvfo1RWturhyAEkBRDQ9OfqWqvVPEsE3A/aARo+sn7j\n\
9vauQe7m3INmxYKGXVDWKoNNOoSvb5C56wbYx36+8Bamxua7/ga2PiSFlhp+ajn4\n\
mknRM+YQI3SFrS0LV/Fgwb+SL8nLMp0QRKna1Wz7l8T4Fa3SSfwe+YZDCy35EJaj\n\
b6gknHVgavtEVT+zgvzcQ+FTBRoJJCNHSygvBbaOCRBwEk4nvCgPpG415m20Toev\n\
5AVQvG2FC4CH5KMa7ntAiWvDPRWYh4VWkDQiDEEhKgDCq5AMjRgZexHD3V7IrdG3\n\
JQIDAQAB\n\
-----END PUBLIC KEY-----");

static
void
test_operate_import()
{
  elle::String const passphrase = "Sancho";

  // Load RSA public key.
  boost::filesystem::path path_public_key =
    _temporary(public_key);

  infinit::cryptography::rsa::PublicKey K =
    infinit::cryptography::rsa::pem::import_K(path_public_key);

  boost::filesystem::remove(path_public_key);

  // 1) Try to load the RSA private key with an incorrect
  // passphrase 2) Load RSA private key in its encrypted form.
  boost::filesystem::path path_private_key_encrypted =
    _temporary(private_key_encrypted);

  // 1)
  BOOST_CHECK_THROW(
    infinit::cryptography::rsa::pem::import_k(path_private_key_encrypted,
                                              "wrong passphrase"),
    infinit::cryptography::Exception);

  // 2)
  infinit::cryptography::rsa::PrivateKey k =
    infinit::cryptography::rsa::pem::import_k(path_private_key_encrypted,
                                              passphrase);

  boost::filesystem::remove(path_private_key_encrypted);

  // Load RSA keypair through the private key in its unencrypted form.
  boost::filesystem::path path_private_key_unencrypted =
    _temporary(private_key_unencrypted);

  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::pem::import_keypair(
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
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);

  boost::filesystem::path path = _temporary();

  elle::String const passphrase = "Dave";

  // Export keypair.
  infinit::cryptography::rsa::pem::export_keypair(
    keypair,
    path,
    passphrase,
    infinit::cryptography::Cipher::aes256,
    infinit::cryptography::Mode::cbc);

  // 1) Try to re-import with wrong passphrase 3) Re-import private key.

  // 1)
  BOOST_CHECK_THROW(
    infinit::cryptography::rsa::pem::import_k(path,
                                              "wrong passphrase"),
    infinit::cryptography::Exception);

  // 2)
  infinit::cryptography::rsa::PrivateKey k =
    infinit::cryptography::rsa::pem::import_k(path,
                                              passphrase);

  BOOST_CHECK_EQUAL(keypair.k(), k);

  // Try to import only the public part of the key from a private key file
  // which is encrypted.
  BOOST_CHECK_THROW(
    infinit::cryptography::rsa::pem::import_K(path),
    infinit::cryptography::Exception);

  // Extract the public key from the private key.
  infinit::cryptography::rsa::PublicKey K(k,
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/pem");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
  suite->add(BOOST_TEST_CASE(test_operate));
#endif

  boost::unit_test::framework::master_test_suite().add(suite);
}
