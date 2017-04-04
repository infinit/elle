#ifndef ELLE_CRYPTOGRAPHY_HASH_HH
# define ELLE_CRYPTOGRAPHY_HASH_HH

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Oneway.hh>

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
    /// Hash blocks of data (last block empty) and return a digest message.
    elle::Buffer
    hash(std::function<elle::ConstWeakBuffer (void)> next_block,
         Oneway const oneway);
    /// Hash an input stream and return a digest message.
    elle::Buffer
    hash(std::istream& plain,
         Oneway const oneway);
  }
}

#endif
