#ifndef INFINIT_CRYPTOGRAPHY_CRYPTOSYSTEM_HH
# define INFINIT_CRYPTOGRAPHY_CRYPTOSYSTEM_HH

# include <iosfwd>

# include <elle/serialization/Serializer.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the asymmetric cryptosystem algorithm.
    ///
    /// Note that the asymmetric cryptosystems do not provide stream-based
    /// cryptographic methods (encryption, signature etc.) unlike symmetric
    /// and oneway algorithms. Consider creating the envelope yourself should
    /// you want to encrypt a large amount of data: generate a temporary
    /// key, encrypt the data with a symmetric algorithm (e.g AES) and encrypt
    /// the temporary key with the asymmetric algorithm.
    enum class Cryptosystem
    {
      rsa,
      dh,
      dsa,
      ec
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Cryptosystem const cryptosystem);
  }
}

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<infinit::cryptography::Cryptosystem>
    {
      typedef elle::Natural8 Type;
      static
      elle::Natural8
      convert(infinit::cryptography::Cryptosystem const& value);
      static
      infinit::cryptography::Cryptosystem
      convert(elle::Natural8 const& representation);
    };
  }
}

#endif
