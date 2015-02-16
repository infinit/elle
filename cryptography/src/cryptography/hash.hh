#ifndef CRYPTOGRAPHY_HASH_HH
# define CRYPTOGRAPHY_HASH_HH

# include <elle/Buffer.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace hash
    {
      elle::Buffer
      sha256(elle::ConstWeakBuffer const& input);
    }
  }
}

#endif
