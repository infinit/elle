
#include <iostream>

#include <elle/Elle.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Cipher.hh>


int main()
{
  assert(elle::Elle::Initialize() == elle::Status::Ok);

  elle::cryptography::SecretKey secret_key;

  assert(secret_key.Generate() == elle::Status::Ok);

  elle::cryptography::Cipher cipher;

  std::string const secret_string =
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
  ;

  assert(secret_key.Encrypt(secret_string, cipher) == elle::Status::Ok);

    {
      std::string res;
      assert(secret_key.Decrypt(cipher, res) == elle::Status::Ok);
      std::cout << res << std::endl << secret_string << std::endl;

      assert(res == secret_string);
    }

  std::cout << "tests done.\n";

  return 0;

}
