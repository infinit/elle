#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Error.hh>
#include <elle/log.hh>

#include <openssl/evp.h>

namespace elle
{
  namespace cryptography
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Oneway const oneway)
    {
      switch (oneway)
      {
      case Oneway::md5:
        return stream << "md5";
      case Oneway::sha:
        return stream << "sha";
      case Oneway::sha1:
        return stream << "sha1";
      case Oneway::sha224:
        return stream << "sha224";
      case Oneway::sha256:
        return stream << "sha256";
      case Oneway::sha384:
        return stream << "sha384";
      case Oneway::sha512:
        return stream << "sha512";
      }
      elle::unreachable();
    }

    namespace oneway
    {
      /*----------.
      | Functions |
      `----------*/

      ::EVP_MD const*
      resolve(Oneway const name)
      {
        switch (name)
        {
        case Oneway::md5:
          return (::EVP_md5());
        case Oneway::sha:
          return (::EVP_sha());
        case Oneway::sha1:
          return (::EVP_sha1());
        case Oneway::sha224:
          return (::EVP_sha224());
        case Oneway::sha256:
          return (::EVP_sha256());
        case Oneway::sha384:
          return (::EVP_sha384());
        case Oneway::sha512:
          return (::EVP_sha512());
        }
        elle::unreachable();
      }

      Oneway
      resolve(::EVP_MD const* function)
      {
        static auto const functions =
          std::map< ::EVP_MD const*, Oneway>
          {
            { ::EVP_md5(), Oneway::md5 },
            { ::EVP_sha(), Oneway::sha },
            { ::EVP_sha1(), Oneway::sha1 },
            { ::EVP_sha224(), Oneway::sha224 },
            { ::EVP_sha256(), Oneway::sha256 },
            { ::EVP_sha384(), Oneway::sha384 },
            { ::EVP_sha512(), Oneway::sha512 },
          };

        auto it = functions.find(function);
        if (it == functions.end())
          throw Error(elle::sprintf("unable to resolve the given one-way "
                                    "function '%s'", function));
        else
          return it->second;
      }
    }
  }
}

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    uint8_t
    Serialize<elle::cryptography::Oneway>::convert(
      elle::cryptography::Oneway const& value)
    {
      return static_cast<uint8_t>(value);
    }

    elle::cryptography::Oneway
    Serialize<elle::cryptography::Oneway>::convert(
      uint8_t const& representation)
    {
      return static_cast<elle::cryptography::Oneway>(representation);
    }
  }
}
