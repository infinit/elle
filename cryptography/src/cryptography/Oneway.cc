#include <cryptography/Oneway.hh>
#include <cryptography/Error.hh>

#include <elle/log.hh>

namespace infinit
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
        {
          stream << "md5";
          break;
        }
        case Oneway::sha:
        {
          stream << "sha";
          break;
        }
        case Oneway::sha1:
        {
          stream << "sha1";
          break;
        }
        case Oneway::sha224:
        {
          stream << "sha224";
          break;
        }
        case Oneway::sha256:
        {
          stream << "sha256";
          break;
        }
        case Oneway::sha384:
        {
          stream << "sha384";
          break;
        }
        case Oneway::sha512:
        {
          stream << "sha512";
          break;
        }
        default:
          throw Error(elle::sprintf("unknown one-way algorithm '%s'",
                                    static_cast<int>(oneway)));
      }

      return (stream);
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
          default:
            throw Error(elle::sprintf("unable to resolve the given one-way "
                                      "function name '%s'", name));
        }

        elle::unreachable();
      }

      Oneway
      resolve(::EVP_MD const* function)
      {
        static std::map< ::EVP_MD const*, Oneway > const functions =
          {
            { ::EVP_md5(), Oneway::md5 },
            { ::EVP_sha(), Oneway::sha },
            { ::EVP_sha1(), Oneway::sha1 },
            { ::EVP_sha224(), Oneway::sha224 },
            { ::EVP_sha256(), Oneway::sha256 },
            { ::EVP_sha384(), Oneway::sha384 },
            { ::EVP_sha512(), Oneway::sha512 },
          };

        for (auto const& iterator: functions)
        {
          if (function == iterator.first)
            return (iterator.second);
        }

        throw Error(elle::sprintf("unable to resolve the given one-way "
                                  "function '%s'", function));
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
    Serialize<infinit::cryptography::Oneway>::convert(
      infinit::cryptography::Oneway const& value)
    {
      return (static_cast<uint8_t>(value));
    }

    infinit::cryptography::Oneway
    Serialize<infinit::cryptography::Oneway>::convert(
      uint8_t const& representation)
    {
      return (static_cast<infinit::cryptography::Oneway>(representation));
    }
  }
}
