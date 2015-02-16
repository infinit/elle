#ifndef INFINIT_CRYPTOGRAPHY_BN_HH
# define INFINIT_CRYPTOGRAPHY_BN_HH

# include <iosfwd>

# include <openssl/bn.h>

# include <elle/serialization/Serializer.hh>

/*----------.
| Operators |
`----------*/


std::ostream&
operator <<(std::ostream& stream,
            ::BIGNUM const& bignum);

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<BIGNUM*>
    {
      typedef std::string Type;
      static
      std::string
      convert(BIGNUM*& bignum);
      static
      BIGNUM*
      convert(std::string& repr);
    };
  }
}

# include <cryptography/bn.hxx>

#endif
