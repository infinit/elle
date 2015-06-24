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
