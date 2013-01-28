#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <elle/assert.hh>

#include <iostream>

void test_encrypt()
{
  std::string const my_secret_text =
    "This is a very secret text. my gmail password is 'bite'";

  cryptography::KeyPair pair =
    cryptography::KeyPair::generate(cryptography::Cryptosystem::rsa, 1024);

  cryptography::Code code = pair.K().encrypt(my_secret_text);

  std::cout << "encrypted size: " << code.buffer().size() << "\n";

  std::string res = pair.k().decrypt<std::string>(code);
}

int main()
{
  test_encrypt();

  std::cout << "tests done.\n";
  return 0;
}
