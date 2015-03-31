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
        case Cipher::null:
        {
          stream << "null";
          break;
        }
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
          throw Exception(elle::sprintf("unknown cipher '%s'",
                                        static_cast<int>(cipher)));
      }

      return (stream);
    }

    std::ostream&
    operator <<(std::ostream& stream,
                Mode const mode)
    {
      switch (mode)
      {
        case Mode::none:
        {
          stream << "none";
          break;
        }
        case Mode::cbc:
        {
          stream << "cbc";
          break;
        }
        case Mode::ecb:
        {
          stream << "ecb";
          break;
        }
        case Mode::cfb:
        {
          stream << "cfb";
          break;
        }
        case Mode::ofb:
        {
          stream << "ofb";
          break;
        }
        default:
          throw Exception(elle::sprintf("unknown operation mode '%s'",
                                        static_cast<int>(mode)));
      }

      return (stream);
    }

    namespace cipher
    {
      /*----------.
      | Functions |
      `----------*/

      ::EVP_CIPHER const*
      resolve(Cipher const cipher,
              Mode const mode)
      {
        ELLE_TRACE_FUNCTION(cipher, mode);

        switch (cipher)
        {
          case Cipher::null:
          {
            switch (mode)
            {
              case Mode::none:
                return (::EVP_enc_null());
              default:
                break;
            }

            break;
          }
          case Cipher::des:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_des_cbc());
              case Mode::ecb:
                return (::EVP_des_ecb());
              case Mode::cfb:
                return (::EVP_des_cfb());
              case Mode::ofb:
                return (::EVP_des_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::des2:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_des_ede_cbc());
              case Mode::ecb:
                return (::EVP_des_ede_ecb());
              case Mode::cfb:
                return (::EVP_des_ede_cfb());
              case Mode::ofb:
                return (::EVP_des_ede_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::des3:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_des_ede3_cbc());
              case Mode::ecb:
                return (::EVP_des_ede3_ecb());
              case Mode::cfb:
                return (::EVP_des_ede3_cfb());
              case Mode::ofb:
                return (::EVP_des_ede3_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::desx:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_desx_cbc());
              default:
                break;
            }

            break;
          }
          case Cipher::idea:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_idea_cbc());
              case Mode::ecb:
                return (::EVP_idea_ecb());
              case Mode::cfb:
                return (::EVP_idea_cfb());
              case Mode::ofb:
                return (::EVP_idea_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::rc2:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_rc2_cbc());
              case Mode::ecb:
                return (::EVP_rc2_ecb());
              case Mode::cfb:
                return (::EVP_rc2_cfb());
              case Mode::ofb:
                return (::EVP_rc2_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::blowfish:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_bf_cbc());
              case Mode::ecb:
                return (::EVP_bf_ecb());
              case Mode::cfb:
                return (::EVP_bf_cfb());
              case Mode::ofb:
                return (::EVP_bf_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::cast5:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_cast5_cbc());
              case Mode::ecb:
                return (::EVP_cast5_ecb());
              case Mode::cfb:
                return (::EVP_cast5_cfb());
              case Mode::ofb:
                return (::EVP_cast5_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::aes128:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_aes_128_cbc());
              case Mode::ecb:
                return (::EVP_aes_128_ecb());
              case Mode::cfb:
                return (::EVP_aes_128_cfb());
              case Mode::ofb:
                return (::EVP_aes_128_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::aes192:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_aes_192_cbc());
              case Mode::ecb:
                return (::EVP_aes_192_ecb());
              case Mode::cfb:
                return (::EVP_aes_192_cfb());
              case Mode::ofb:
                return (::EVP_aes_192_ofb());
              default:
                break;
            }

            break;
          }
          case Cipher::aes256:
          {
            switch (mode)
            {
              case Mode::cbc:
                return (::EVP_aes_256_cbc());
              case Mode::ecb:
                return (::EVP_aes_256_ecb());
              case Mode::cfb:
                return (::EVP_aes_256_cfb());
              case Mode::ofb:
                return (::EVP_aes_256_ofb());
              default:
                break;
            }

            break;
          }
          default:
            throw Exception(elle::sprintf("unknown cipher '%s'", cipher));
        }

        throw Exception(
          elle::sprintf("the operation mode '%s' is "
                        "non-compliant with the cipher '%s'",
                        mode, cipher));
      }
    }
  }
}
