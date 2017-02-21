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

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/random.hh>
# include <elle/cryptography/SecretKey.hh>
# include <elle/cryptography/hash.hh>
# include <elle/cryptography/rsa/KeyPair.hh>
# include <elle/cryptography/dsa/KeyPair.hh>

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

      elle::cryptography::rsa::KeyPair keypair =
        elle::cryptography::rsa::keypair::generate(2048);

      elle::Buffer code = keypair.K().seal(data);
      elle::Buffer clear = keypair.k().open(code);

      elle::fprintf(std::cout, "1) %s\n", clear.string());
    }

    // Sign/verify with DSA.
    {
      std::string data("lap'ara'deuh michel!");

      elle::cryptography::dsa::KeyPair keypair =
        elle::cryptography::dsa::keypair::generate(1024);

      elle::Buffer signature = keypair.k().sign(data);

      if (keypair.K().verify(signature, data) == true)
        elle::fprintf(std::cout, "2) signature valid\n");
      else
        elle::fprintf(std::cout, "2) signature invalid\n");
    }
  }

  /*---------------------.
  | Symmetric encryption |
  `---------------------*/
  {
    // Encrypt/decrypt with AES-128.
    std::string data("Answer to The Ultimate Question of Life, "
                     "the Universe, and Everything");

    elle::cryptography::SecretKey key =
      elle::cryptography::secretkey::generate(256);

    elle::Buffer code = key.encipher(data,
                                     elle::cryptography::Cipher::aes128);
    elle::Buffer clear = key.decipher(code,
                                      elle::cryptography::Cipher::aes128);

    elle::fprintf(std::cout, "3) %s\n", clear.string());
  }

  /*-----.
  | Hash |
  `-----*/
  {
    // Hash with SHA-1
    std::string data("mange mon fion");

    elle::Buffer digest =
      elle::cryptography::hash(data,
                                  elle::cryptography::Oneway::sha1);

    elle::fprintf(std::cout, "4) %s\n",
                  elle::format::hexadecimal::encode(digest));
  }

  return (0);
}
