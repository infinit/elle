#pragma once

#include <iosfwd>

#include <elle/serialization/Serializer.hh>

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
      using Type = uint8_t;
      static
      uint8_t
      convert(elle::cryptography::Cryptosystem const& value);
      static
      elle::cryptography::Cryptosystem
      convert(uint8_t const& representation);
    };
  }
}
