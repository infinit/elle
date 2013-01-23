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
//       cryptography::SecretKey key("password");
//       elle::io::Unique unique;
//       to_string<OutputBase64Archive>(unique) << key;
//       std::cout << unique << std::endl;

  std::string reference("AAAAAAgAAAAAAAAAcGFzc3dvcmQ=");

  cryptography::SecretKey key("password");
  elle::io::Unique unique;
  to_string<OutputBase64Archive>(unique) << key;

  ELLE_ASSERT(unique == reference);

  std::cout << "tests done." << std::endl;
  return 0;
}


