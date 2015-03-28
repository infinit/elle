#include <cryptography/rsa/Padding.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>
#include <elle/log.hh>

#include <openssl/rsa.h>
#include <openssl/err.h>

#include <ostream>

ELLE_LOG_COMPONENT("infinit.cryptography.rsa.Padding");

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /*----------.
      | Operators |
      `----------*/

      std::ostream&
      operator <<(std::ostream& stream,
                  Padding const padding)
      {
        switch (padding)
        {
          case Padding::none:
          {
            stream << "none";
            break;
          }
          case Padding::pkcs1:
          {
            stream << "pkcs1";
            break;
          }
          case Padding::sslv23:
          {
            stream << "sslv23";
            break;
          }
          case Padding::oaep:
          {
            stream << "oaep";
            break;
          }
          case Padding::x931:
          {
            stream << "x931";
            break;
          }
          case Padding::pss:
          {
            stream << "pss";
            break;
          }
          default:
            throw Exception(
              elle::sprintf("unknown of unssuported padding algorithm '%s'",
                            padding));
        }

        return (stream);
      }

      namespace padding
      {
        /*----------.
        | Functions |
        `----------*/

        int
        resolve(Padding const name)
        {
          ELLE_TRACE_FUNCTION(name);

          switch (name)
          {
            case Padding::none:
              return (RSA_NO_PADDING);
            case Padding::pkcs1:
              return (RSA_PKCS1_PADDING);
            case Padding::sslv23:
              return (RSA_SSLV23_PADDING);
            case Padding::oaep:
              return (RSA_PKCS1_OAEP_PADDING);
            case Padding::x931:
              return (RSA_X931_PADDING);
            case Padding::pss:
              return (RSA_PKCS1_PSS_PADDING);
            default:
              throw Exception(elle::sprintf("unable to resolve the given "
                                            "padding name '%s'", name));
          }

          elle::unreachable();
        }

        Padding
        resolve(int const value)
        {
          ELLE_TRACE_FUNCTION(value);

          switch (value)
          {
            case RSA_NO_PADDING:
              return (Padding::none);
            case RSA_PKCS1_PADDING:
              return (Padding::pkcs1);
            case RSA_SSLV23_PADDING:
              return (Padding::sslv23);
            case RSA_PKCS1_OAEP_PADDING:
              return (Padding::oaep);
            case RSA_X931_PADDING:
              return (Padding::x931);
            case RSA_PKCS1_PSS_PADDING:
              return (Padding::pss);
            default:
              throw Exception(elle::sprintf("unable to resolve the given "
                                            "padding value '%s'", value));
          }

          elle::unreachable();
        }

        void
        print(std::ostream& stream,
              ::EVP_PKEY_CTX* context)
        {
          int padding;

          if (EVP_PKEY_CTX_get_rsa_padding(context,
                                           static_cast<void*>(&padding)) <= 0)
            throw Exception(
              elle::sprintf("unable to retrieve the RSA padding from "
                            "EVP_PKEY context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          stream << resolve(padding);
        }

        elle::Natural32
        footprint(::EVP_PKEY_CTX* context)
        {
          int padding;

          if (EVP_PKEY_CTX_get_rsa_padding(context,
                                           static_cast<void*>(&padding)) <= 0)
            throw Exception(
              elle::sprintf("unable to retrieve the RSA padding from "
                            "EVP_PKEY context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Unfortunately, OpenSSL does not seem to provide an easy way to
          // retrieve the padding size depending on the algorithm.
          switch (padding)
          {
            case RSA_PKCS1_PADDING:
              // RSA_PKCS1_PADDING_SIZE = 11 bytes.
              return (88);
            case RSA_SSLV23_PADDING:
              // 11 bytes.
              return (88);
            case RSA_NO_PADDING:
              return (0);
            case RSA_PKCS1_OAEP_PADDING:
              // 1 + 2 x SHA_DIGEST_LENGHT + 1 bytes = 42 bytes.
              return (336);
            case RSA_X931_PADDING:
              // 1 header nibble + 1 padding nibble + 2 trailer
              // bytes = 3 bytes.
              return (24);
            default:
              throw Exception(
                elle::sprintf("unknown of unsupported padding algorithm '%s'",
                              padding));
          }
        }
      }
    }
  }
}
