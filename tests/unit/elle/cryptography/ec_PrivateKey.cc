#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <elle/idiom/Open.hh>

#include <iostream>

#define show_assert(expr)                                                     \
  do {                                                                        \
    if (expr != elle::Status::Ok) { assert(false && #expr);}                  \
  } while (false)                                                             \

void test_encrypt()
{
  std::string const my_secret_text =
    "This is a very secret text. my gmail password is 'bite'"
  ;

  cryptography::Code code;

  cryptography::KeyPair pair(cryptography::KeyPair::generate());

  show_assert(pair.K.Encrypt(my_secret_text, code));

  std::cout << "encrypted size: " << code.region.size << "\n";

  std::string res;
  show_assert(pair.k.Decrypt(code, res));
}

int main()
{
  test_encrypt();

  std::cout << "tests done.\n";
  return 0;
}
