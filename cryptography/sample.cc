// This program is intended to show this library can be used in its simplest
// way.
//
// $ ./sample
// 1) chang'daile
// 2) signature valid
// 3) Answer to The Ultimate Question of Life, the Universe, and Everything
// 4) 76b4075f7b0aab59942335e4497b42ecfa9e3e0f
// $>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/format/hexadecimal.hh>

# include <cryptography/fwd.hh>
# include <cryptography/random.hh>
# include <cryptography/SecretKey.hh>
# include <cryptography/rsa/KeyPair.hh>
# include <cryptography/dsa/KeyPair.hh>

# include <iostream>

int
main()
{
  /*----------------------.
  | Asymmetric encryption |
  `----------------------*/
  {
    // Seal/open with RSA.
    {
      std::string data("chang'daile!");

      infinit::cryptography::rsa::KeyPair keypair =
        infinit::cryptography::rsa::keypair::generate(2048);

      elle::Buffer code = keypair.K().seal(data);
      elle::Buffer clear = keypair.k().open(code);

      elle::printf("1) %s\n", clear.string());
    }

    // Sign/verify with DSA.
    {
      std::string data("lap'ara'deuh michel!");

      infinit::cryptography::dsa::KeyPair keypair =
        infinit::cryptography::dsa::keypair::generate(1024);

      elle::Buffer signature = keypair.k().sign(data);

      if (keypair.K().verify(signature, data) == true)
        elle::printf("2) signature valid\n");
      else
        elle::printf("2) signature invalid\n");
    }
  }

  /*---------------------.
  | Symmetric encryption |
  `---------------------*/
  {
    // Encrypt/decrypt with AES-128.
    std::string data("Answer to The Ultimate Question of Life, "
                     "the Universe, and Everything");

    infinit::cryptography::SecretKey key =
      infinit::cryptography::secretkey::generate(256);

    elle::Buffer code = key.encipher(data,
                                     infinit::cryptography::Cipher::aes128);
    elle::Buffer clear = key.decipher(code,
                                      infinit::cryptography::Cipher::aes128);

    elle::printf("3) %s\n", clear.string());
  }

  /*-----.
  | Hash |
  `-----*/
  {
    // Hash with SHA-1
    std::string data("mange mon fion");

    elle::Buffer digest =
      infinit::cryptography::hash(data,
                                  infinit::cryptography::Oneway::sha1);

    elle::printf("4) %s\n", elle::format::hexadecimal::encode(digest.buffer()));
  }

  return (0);
}
