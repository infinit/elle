#ifndef INFINIT_CRYPTOGRAPHY_HASH_HH
# define INFINIT_CRYPTOGRAPHY_HASH_HH

# include <cryptography/fwd.hh>
# include <cryptography/Oneway.hh>

# include <elle/types.hh>

# include <openssl/evp.h>

# include <iosfwd>

namespace infinit
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
