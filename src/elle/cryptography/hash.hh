#ifndef ELLE_CRYPTOGRAPHY_HASH_HH
# define ELLE_CRYPTOGRAPHY_HASH_HH

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Oneway.hh>

# include <elle/types.hh>

# include <openssl/evp.h>

# include <iosfwd>

namespace elle
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/
    /// Hash a plain text and return a digest message.
    elle::Buffer
    hash(elle::ConstWeakBuffer const& plain,
         Oneway const oneway);
    /// Hash an input stream and return a digest message.
    elle::Buffer
    hash(std::istream& plain,
         Oneway const oneway);
  }
}

#endif
