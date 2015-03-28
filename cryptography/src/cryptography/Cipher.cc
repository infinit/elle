#include <cryptography/cipher.hh>
#include <cryptography/Exception.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.cipher");

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Cipher const cipher)
    {
      switch (cipher)
      {
        case Cipher::des:
        {
          stream << "des";
          break;
        }
        case Cipher::des2:
        {
          stream << "des2";
          break;
        }
        case Cipher::des3:
        {
          stream << "des3";
          break;
        }
        case Cipher::desx:
        {
          stream << "desx";
          break;
        }
        case Cipher::idea:
        {
          stream << "idea";
          break;
        }
        case Cipher::rc2:
        {
          stream << "rc2";
          break;
        }
        case Cipher::blowfish:
        {
          stream << "blowfish";
          break;
        }
        case Cipher::cast5:
        {
          stream << "cast5";
          break;
        }
        case Cipher::aes128:
        {
          stream << "aes128";
          break;
        }
        case Cipher::aes192:
        {
          stream << "aes192";
          break;
        }
        case Cipher::aes256:
        {
          stream << "aes256";
          break;
        }
        default:
          throw Exception(elle::sprintf("unknown cipher cipher '%s'",
                                        static_cast<int>(cipher)));
      }

      return (stream);
    }

    namespace cipher
    {
      /*----------.
      | Functions |
      `----------*/

      ::EVP_CIPHER const*
      resolve(Cipher const name)
      {
        ELLE_TRACE_FUNCTION(name);

        switch (name)
        {
          case Cipher::des:
            return (::EVP_des_cbc());
          case Cipher::des2:
            return (::EVP_des_ede_cbc());
          case Cipher::des3:
            return (::EVP_des_ede3_cbc());
          case Cipher::desx:
            return (::EVP_desx_cbc());
          case Cipher::idea:
            return (::EVP_idea_cbc());
          case Cipher::rc2:
            return (::EVP_rc2_cbc());
          case Cipher::blowfish:
            return (::EVP_bf_cbc());
          case Cipher::cast5:
            return (::EVP_cast5_cbc());
          case Cipher::aes128:
            return (::EVP_aes_128_cbc());
          case Cipher::aes192:
            return (::EVP_aes_192_cbc());
          case Cipher::aes256:
            return (::EVP_aes_256_cbc());
          default:
            throw Exception(elle::sprintf("unable to resolve the given cipher "
                                          "function name '%s'", name));
        }

        elle::unreachable();
      }
    }
  }
}
