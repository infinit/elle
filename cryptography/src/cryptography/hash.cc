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
  }
}
