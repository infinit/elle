// This program is intended to show this library can be used in its simplest
// way.
//
// $ ./sample
// 1) i = 42
// 2) signature valid
// 3) Answer to The Ultimate Question of Life, the Universe, and Everything
// 4) 8cd41971112b255c3106f073d7a010f672af99b3
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
# include <elle/serialization/Serializer.hh>

# include <iostream>

//
// ---------- Sample Data Structure -------------------------------------------
//

class Sample
{
  /*-------------.
  | Construction |
  `-------------*/
public:
  Sample(std::string const& string):
    _salt(0.91283173),
    _string(string)
  {
  }

  Sample(elle::serialization::SerializerIn& serializer)
  {
    this->serialize(serializer);
  }

  void
  Sample(elle::serialization::Serializer& serializer)
  {
    serializer.serialize("salt", this->_salt);
    serializer.serialize("string", this->_string);
  }

  Sample(Sample const&) = default;

  /*-----------.
  | Attributes |
  `-----------*/
private:
  ELLE_ATTRIBUTE_R(double, salt);
  ELLE_ATTRIBUTE_R(std::string, string);
};

//
// ---------- Main ------------------------------------------------------------
//

int
main()
{
  Sample sample("this is a sample complex data structure");

  /*----------------------.
  | Asymmetric encryption |
  `----------------------*/
  {
    // Encrypt/decrypt with RSA.
    {
      infinit::cryptography::rsa::KeyPair keypair =
        infinit::cryptography::rsa::keypair::generate(2048);

      infinit::cryptography::Code code = keypair.K().encrypt(42);

      uint32_t i = keypair.k().decrypt<uint32_t>(code);

      elle::printf("1) i = %s\n", i);
    }

    // Sign/verify with DSA.
    {
      infinit::cryptography::dsa::KeyPair keypair =
        infinit::cryptography::dsa::keypair::generate(1024);

      infinit::cryptography::Signature signature = keypair.k().sign(sample);

      if (keypair.K().verify(signature, sample) == true)
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
    infinit::cryptography::SecretKey key =
      infinit::cryptography::secretkey::generate(
        256,
        infinit::cryptography::Cipher::aes128);

    std::string data("Answer to The Ultimate Question of Life, "
                     "the Universe, and Everything");

    infinit::cryptography::Code code = key.encrypt(data);
    std::string _data = key.decrypt<std::string>(code);

    elle::printf("3) %s\n", _data);
  }

  /*-----.
  | Hash |
  `-----*/
  {
    // Hash with SHA-1
    infinit::cryptography::Digest digest =
      infinit::cryptography::hash(sample,
                                  infinit::cryptography::Oneway::sha1);

    elle::printf("4) %s\n", elle::format::hexadecimal::encode(digest.buffer()));
  }

  return (0);
}
