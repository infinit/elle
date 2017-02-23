#ifndef ELLE_CRYPTOGRAPHY_CRYPTOSYSTEM_HH
# define ELLE_CRYPTOGRAPHY_CRYPTOSYSTEM_HH

# include <iosfwd>

# include <elle/serialization/Serializer.hh>

namespace elle
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
    struct Serialize<elle::cryptography::Cryptosystem>
    {
      typedef uint8_t Type;
      static
      uint8_t
      convert(elle::cryptography::Cryptosystem const& value);
      static
      elle::cryptography::Cryptosystem
      convert(uint8_t const& representation);
    };
  }
}

#endif
