#include <cryptography/cipher.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.cipher");

namespace infinit
{
  namespace cryptography
  {
    namespace cipher
    {
      /*----------.
      | Functions |
      `----------*/

      ::EVP_CIPHER const*
      resolve(Algorithm const name)
      {
        ELLE_TRACE_FUNCTION(name);

        switch (name)
          {
          case Algorithm::des:
            return (::EVP_des_cbc());
          case Algorithm::des2:
            return (::EVP_des_ede_cbc());
          case Algorithm::des3:
            return (::EVP_des_ede3_cbc());
          case Algorithm::desx:
            return (::EVP_desx_cbc());
          case Algorithm::idea:
            return (::EVP_idea_cbc());
          case Algorithm::rc2:
            return (::EVP_rc2_cbc());
          case Algorithm::blowfish:
            return (::EVP_bf_cbc());
          case Algorithm::cast5:
            return (::EVP_cast5_cbc());
          case Algorithm::aes128:
            return (::EVP_aes_128_cbc());
          case Algorithm::aes192:
            return (::EVP_aes_192_cbc());
          case Algorithm::aes256:
            return (::EVP_aes_256_cbc());
          default:
            throw elle::Exception("unable to resolve the given cipher "
                                  "function name '%s'", name);
          }

        elle::unreachable();
      }

      /*----------.
      | Operators |
      `----------*/

      std::ostream&
      operator <<(std::ostream& stream,
                  Algorithm const algorithm)
      {
        switch (algorithm)
          {
          case Algorithm::des:
            {
              stream << "des";
              break;
            }
          case Algorithm::des2:
            {
              stream << "des2";
              break;
            }
          case Algorithm::des3:
            {
              stream << "des3";
              break;
            }
          case Algorithm::desx:
            {
              stream << "desx";
              break;
            }
          case Algorithm::idea:
            {
              stream << "idea";
              break;
            }
          case Algorithm::rc2:
            {
              stream << "rc2";
              break;
            }
          case Algorithm::blowfish:
            {
              stream << "blowfish";
              break;
            }
          case Algorithm::cast5:
            {
              stream << "cast5";
              break;
            }
          case Algorithm::aes128:
            {
              stream << "aes128";
              break;
            }
          case Algorithm::aes192:
            {
              stream << "aes192";
              break;
            }
          case Algorithm::aes256:
            {
              stream << "aes256";
              break;
            }
          default:
            throw elle::Exception("unknown cipher algorithm '%s'",
                                  static_cast<int>(algorithm));
          }

        return (stream);
      }
    }
  }
}
