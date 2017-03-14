#pragma once

#include <openssl/bn.h>

#include <elle/serialization/Serializer.hh>
#include <elle/operator.hh>

#include <iosfwd>
#include <utility>

ELLE_OPERATOR_RELATIONALS();

/*----------.
| Operators |
`----------*/

std::ostream&
operator <<(std::ostream& stream,
            ::BIGNUM const& bignum);

bool
operator ==(::BIGNUM const& a,
            ::BIGNUM const& b);
bool
operator <(::BIGNUM const& a,
           ::BIGNUM const& b);

/*--------------.
| Serialization |
`--------------*/

// Note that another specialization for BIGNUM-smartpointers
// should also be written.
namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<BIGNUM*>
    {
      using Type = elle::Buffer;
      static
      elle::Buffer
      convert(::BIGNUM* const& value);
      static
      ::BIGNUM*
      convert(elle::Buffer const& representation);
    };
  }
}
