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
    /// Hash an input stream and return a digest message.
    elle::Buffer
    hash(std::istream& plain,
         Oneway const oneway);
    /// Initialize a hash manually.
    ::EVP_MD_CTX
    hash_init(Oneway const oneway);
    /// Update a manual hash with data.
    void
    hash_update(::EVP_MD_CTX* context, elle::Buffer const& buffer);
    /// Finalize a manual hash.
    elle::Buffer
    hash_finalize(::EVP_MD_CTX* context);
  }
}

#endif
