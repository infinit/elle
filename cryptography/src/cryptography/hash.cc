#include <cryptography/hash.hh>
#include <cryptography/raw.hh>

#include <elle/Buffer.hh>
#include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    elle::Buffer
    hash(elle::ConstWeakBuffer const& plain,
         Oneway const oneway)
    {
      elle::IOStream _plain(plain.istreambuf());

      return (hash(_plain, oneway));
    }

    elle::Buffer
    hash(std::istream& plain,
         Oneway const oneway)
    {

      ::EVP_MD const* function = oneway::resolve(oneway);

      return (raw::hash(function, plain));
    }

    ::EVP_MD_CTX
    hash_init(Oneway const oneway)
    {
      ::EVP_MD const* function = oneway::resolve(oneway);
      return raw::hash_init(function);
    }

    void
    hash_update(::EVP_MD_CTX* context, elle::Buffer const& buffer)
    {
      raw::hash_update(context, buffer);
    }

    elle::Buffer
    hash_finalize(::EVP_MD_CTX* context)
    {
      return raw::hash_finalize(context);
    }
  }
}
