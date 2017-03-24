#pragma once

#include <elle/serialization/Serializer.hh>
#include <elle/cryptography/fwd.hh>
#include <elle/operator.hh>

#include <iosfwd>
#include <utility>

/*----------.
| Operators |
`----------*/

std::ostream&
operator <<(std::ostream& stream,
            BIGNUM const& bignum);

bool
operator ==(BIGNUM const& a,
            BIGNUM const& b);
bool
operator <(BIGNUM const& a,
           BIGNUM const& b);

// XXX: The following operators used to be automatically defined via
// std::rel_ops. However, we should always prefer boost::*_ordered over
// std::rel_ops because it poisons the environment.
// In this case, we have no visibility over BIGNUM, therefore, we have two
// solutions.
// 1. Create a wrapper over BIGNUM.
// 2. Manually define operators.
//
// For now, we chose 2.

bool
operator !=(BIGNUM const& a,
            BIGNUM const& b);
bool
operator >(BIGNUM const& a,
           BIGNUM const& b);

bool
operator <=(BIGNUM const& a,
            BIGNUM const& b);

bool
operator >=(BIGNUM const& a,
            BIGNUM const& b);

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
      convert(BIGNUM* const& value);
      static
      BIGNUM*
      convert(elle::Buffer const& representation);
    };
  }
}
