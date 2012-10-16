#include <iostream>

#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/KeyPair.hh>

#include <elle/idiom/Open.hh>

#define show_assert(expr)                                                     \
  do {                                                                        \
    if (expr != elle::Status::Ok) { assert(false && #expr);}                  \
  } while (false)                                                             \

void test_encrypt()
{
  std::string const my_secret_text =
    "This is a very secret text. my gmail password is 'bite'"
  ;

  elle::cryptography::Code code;

  elle::cryptography::KeyPair pair;

  show_assert(pair.Generate());

  std::cout << "bim\n";

  show_assert(pair.K.Encrypt(my_secret_text, code));

  std::cout << "encrypted size: " << code.region.size << "\n";

    {
      std::string res;
      show_assert(pair.k.Decrypt(code, res));
    }

}

int main()
{
  test_encrypt();

  std::cout << "tests done.\n";
  return 0;
}
