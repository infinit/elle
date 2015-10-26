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
      typedef uint8_t Type;
      static
      uint8_t
      convert(infinit::cryptography::Cryptosystem const& value);
      static
      infinit::cryptography::Cryptosystem
      convert(uint8_t const& representation);
    };
  }
}

#endif
