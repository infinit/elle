
#include <iostream>

#include <elle/Elle.hh>
#include <elle/cryptography/PrivateKey.hh>
#include <elle/cryptography/PublicKey.hh>
#include <elle/cryptography/KeyPair.hh>

#include <elle/idiom/Open.hh>

void test_encrypt()
{
  std::string const my_secret_text =
    "This is a very secret text. my gmail password is 'bite'"
  ;

  elle::cryptography::Code code;

  elle::cryptography::KeyPair pair;

  assert(pair.Generate() == elle::Status::Ok);

  if (pair.k.Encrypt(my_secret_text, code) != elle::Status::Ok)
    {
      show();
      assert(false);
    }

    {
      std::string res;
      assert(pair.k.Decrypt(code, res) == elle::Status::Ok);
    }

}

int main()
{
  elle::Elle::Initialize();
  test_encrypt();

  std::cout << "tests done.\n";
  return 0;
}
