#include <elle/assert.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>

#include <cryptography/SecretKey.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

int main()
{
  using elle::serialize::from_string;
  using elle::serialize::to_string;
  using elle::serialize::InputBase64Archive;
  using elle::serialize::OutputBase64Archive;

  // This string has been generated as follows:
  //
//       cryptography::SecretKey _key(cryptography::cipher::Algorithm::aes256,
//                                    "password");
//       elle::io::Unique _unique;
//       to_string<OutputBase64Archive>(_unique) << _key;
//       std::cout << _unique << std::endl;

  std::string reference("AAAKAAAACAAAAAAAAABwYXNzd29yZAIA");
  cryptography::SecretKey key(cryptography::cipher::Algorithm::aes256,
                              "password");
  elle::io::Unique unique;
  to_string<OutputBase64Archive>(unique) << key;

  ELLE_ASSERT(unique == reference);

  std::cout << "tests done." << std::endl;
  return 0;
}


