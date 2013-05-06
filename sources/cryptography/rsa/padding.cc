#include <cryptography/rsa/padding.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>

#include <openssl/rsa.h>
#include <openssl/err.h>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace padding
      {
        void
        print(std::ostream& stream,
              ::EVP_PKEY_CTX* context)
        {
          int padding;

          if (::EVP_PKEY_CTX_get_rsa_padding(context,
                                             static_cast<void*>(&padding)) <= 0)
            throw Exception(
              elle::sprintf("unable to retrieve the RSA padding from EVP_PKEY "
                            "context: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          switch (padding)
          {
            case RSA_PKCS1_PADDING:
            {
              stream << "pkcs1";
              break;
            }
            case RSA_SSLV23_PADDING:
            {
              stream << "sslv23";
              break;
            }
            case RSA_NO_PADDING:
            {
              stream << "none";
              break;
            }
            case RSA_PKCS1_OAEP_PADDING:
            {
              stream << "oaep";
              break;
            }
            case RSA_X931_PADDING:
            {
              stream << "x931";
              break;
            }
            case RSA_PKCS1_PSS_PADDING:
            {
              stream << "pss";
              break;
            }
            default:
              throw Exception(
                elle::sprintf("unknown padding algorithm '%s'", padding));
          }
        }
      }
    }
  }
}
