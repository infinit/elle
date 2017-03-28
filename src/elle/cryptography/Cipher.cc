#include <openssl/evp.h>

#include <elle/log.hh>

#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/Error.hh>

namespace elle
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
        return stream << "null";
      case Cipher::des:
        return stream << "des";
      case Cipher::des2:
        return stream << "des2";
      case Cipher::des3:
        return stream << "des3";
      case Cipher::desx:
        return stream << "desx";
      case Cipher::idea:
        return stream << "idea";
      case Cipher::rc2:
        return stream << "rc2";
      case Cipher::blowfish:
        return stream << "blowfish";
      case Cipher::cast5:
        return stream << "cast5";
      case Cipher::aes128:
        return stream << "aes128";
      case Cipher::aes192:
        return stream << "aes192";
      case Cipher::aes256:
        return stream << "aes256";
      }
      elle::unreachable();
    }

    std::ostream&
    operator <<(std::ostream& stream,
                Mode const mode)
    {
      switch (mode)
      {
      case Mode::none:
        return stream << "none";
      case Mode::cbc:
        return stream << "cbc";
      case Mode::ecb:
        return stream << "ecb";
      case Mode::cfb:
        return stream << "cfb";
      case Mode::ofb:
        return stream << "ofb";
      }
      elle::unreachable();
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
        switch (cipher)
        {
          case Cipher::null:
          {
            switch (mode)
            {
              case Mode::none:
                return ::EVP_enc_null();
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
                return ::EVP_des_cbc();
              case Mode::ecb:
                return ::EVP_des_ecb();
              case Mode::cfb:
                return ::EVP_des_cfb();
              case Mode::ofb:
                return ::EVP_des_ofb();
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
              return ::EVP_des_ede_cbc();
            case Mode::ecb:
              return ::EVP_des_ede_ecb();
            case Mode::cfb:
              return ::EVP_des_ede_cfb();
            case Mode::ofb:
              return ::EVP_des_ede_ofb();
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
              return ::EVP_des_ede3_cbc();
            case Mode::ecb:
              return ::EVP_des_ede3_ecb();
            case Mode::cfb:
              return ::EVP_des_ede3_cfb();
            case Mode::ofb:
              return ::EVP_des_ede3_ofb();
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
              return ::EVP_desx_cbc();
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
              return ::EVP_idea_cbc();
            case Mode::ecb:
              return ::EVP_idea_ecb();
            case Mode::cfb:
              return ::EVP_idea_cfb();
            case Mode::ofb:
              return ::EVP_idea_ofb();
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
              return ::EVP_rc2_cbc();
            case Mode::ecb:
              return ::EVP_rc2_ecb();
            case Mode::cfb:
              return ::EVP_rc2_cfb();
            case Mode::ofb:
              return ::EVP_rc2_ofb();
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
              return ::EVP_bf_cbc();
            case Mode::ecb:
              return ::EVP_bf_ecb();
            case Mode::cfb:
              return ::EVP_bf_cfb();
            case Mode::ofb:
              return ::EVP_bf_ofb();
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
              return ::EVP_cast5_cbc();
            case Mode::ecb:
              return ::EVP_cast5_ecb();
            case Mode::cfb:
              return ::EVP_cast5_cfb();
            case Mode::ofb:
              return ::EVP_cast5_ofb();
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
              return ::EVP_aes_128_cbc();
            case Mode::ecb:
              return ::EVP_aes_128_ecb();
            case Mode::cfb:
              return ::EVP_aes_128_cfb();
            case Mode::ofb:
              return ::EVP_aes_128_ofb();
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
              return ::EVP_aes_192_cbc();
            case Mode::ecb:
              return ::EVP_aes_192_ecb();
            case Mode::cfb:
              return ::EVP_aes_192_cfb();
            case Mode::ofb:
              return ::EVP_aes_192_ofb();
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
              return ::EVP_aes_256_cbc();
            case Mode::ecb:
              return ::EVP_aes_256_ecb();
            case Mode::cfb:
              return ::EVP_aes_256_cfb();
            case Mode::ofb:
              return ::EVP_aes_256_ofb();
            default:
              break;
            }

            break;
          }
          default:
            throw Error(elle::sprintf("unknown cipher '%s'", cipher));
        }

        throw Error(
          elle::sprintf("the operation mode '%s' is "
                        "non-compliant with the cipher '%s'",
                        mode, cipher));
      }

      std::pair<Cipher, Mode>
      resolve(::EVP_CIPHER const* function)
      {
        static auto const functions =
          std::map<::EVP_CIPHER const*, std::pair<Cipher, Mode>>
          {
            // null
            { ::EVP_enc_null(),
              std::pair<Cipher, Mode>(Cipher::null, Mode::none) },
            // des
            { ::EVP_des_cbc(),
              std::pair<Cipher, Mode>(Cipher::des, Mode::cbc) },
            { ::EVP_des_ecb(),
              std::pair<Cipher, Mode>(Cipher::des, Mode::ecb) },
            { ::EVP_des_cfb(),
              std::pair<Cipher, Mode>(Cipher::des, Mode::cfb) },
            { ::EVP_des_ofb(),
              std::pair<Cipher, Mode>(Cipher::des, Mode::ofb) },
            // des2
            { ::EVP_des_ede_cbc(),
              std::pair<Cipher, Mode>(Cipher::des2, Mode::cbc) },
            { ::EVP_des_ede_ecb(),
              std::pair<Cipher, Mode>(Cipher::des2, Mode::ecb) },
            { ::EVP_des_ede_cfb(),
              std::pair<Cipher, Mode>(Cipher::des2, Mode::cfb) },
            { ::EVP_des_ede_ofb(),
              std::pair<Cipher, Mode>(Cipher::des2, Mode::ofb) },
            // des3
            { ::EVP_des_ede3_cbc(),
              std::pair<Cipher, Mode>(Cipher::des3, Mode::cbc) },
            { ::EVP_des_ede3_ecb(),
              std::pair<Cipher, Mode>(Cipher::des3, Mode::ecb) },
            { ::EVP_des_ede3_cfb(),
              std::pair<Cipher, Mode>(Cipher::des3, Mode::cfb) },
            { ::EVP_des_ede3_ofb(),
              std::pair<Cipher, Mode>(Cipher::des3, Mode::ofb) },
            // desx
            { ::EVP_desx_cbc(),
              std::pair<Cipher, Mode>(Cipher::desx, Mode::cbc) },
            // idea
            { ::EVP_idea_cbc(),
              std::pair<Cipher, Mode>(Cipher::idea, Mode::cbc) },
            { ::EVP_idea_ecb(),
              std::pair<Cipher, Mode>(Cipher::idea, Mode::ecb) },
            { ::EVP_idea_cfb(),
              std::pair<Cipher, Mode>(Cipher::idea, Mode::cfb) },
            { ::EVP_idea_ofb(),
              std::pair<Cipher, Mode>(Cipher::idea, Mode::ofb) },
            // rc2
            { ::EVP_rc2_cbc(),
              std::pair<Cipher, Mode>(Cipher::rc2, Mode::cbc) },
            { ::EVP_rc2_ecb(),
              std::pair<Cipher, Mode>(Cipher::rc2, Mode::ecb) },
            { ::EVP_rc2_cfb(),
              std::pair<Cipher, Mode>(Cipher::rc2, Mode::cfb) },
            { ::EVP_rc2_ofb(),
              std::pair<Cipher, Mode>(Cipher::rc2, Mode::ofb) },
            // blowfish
            { ::EVP_bf_cbc(),
              std::pair<Cipher, Mode>(Cipher::blowfish, Mode::cbc) },
            { ::EVP_bf_ecb(),
              std::pair<Cipher, Mode>(Cipher::blowfish, Mode::ecb) },
            { ::EVP_bf_cfb(),
              std::pair<Cipher, Mode>(Cipher::blowfish, Mode::cfb) },
            { ::EVP_bf_ofb(),
              std::pair<Cipher, Mode>(Cipher::blowfish, Mode::ofb) },
            // cast5
            { ::EVP_cast5_cbc(),
              std::pair<Cipher, Mode>(Cipher::cast5, Mode::cbc) },
            { ::EVP_cast5_ecb(),
              std::pair<Cipher, Mode>(Cipher::cast5, Mode::ecb) },
            { ::EVP_cast5_cfb(),
              std::pair<Cipher, Mode>(Cipher::cast5, Mode::cfb) },
            { ::EVP_cast5_ofb(),
              std::pair<Cipher, Mode>(Cipher::cast5, Mode::ofb) },
            // aes128
            { ::EVP_aes_128_cbc(),
              std::pair<Cipher, Mode>(Cipher::aes128, Mode::cbc) },
            { ::EVP_aes_128_ecb(),
              std::pair<Cipher, Mode>(Cipher::aes128, Mode::ecb) },
            { ::EVP_aes_128_cfb(),
              std::pair<Cipher, Mode>(Cipher::aes128, Mode::cfb) },
            { ::EVP_aes_128_ofb(),
              std::pair<Cipher, Mode>(Cipher::aes128, Mode::ofb) },
            // aes192
            { ::EVP_aes_192_cbc(),
              std::pair<Cipher, Mode>(Cipher::aes192, Mode::cbc) },
            { ::EVP_aes_192_ecb(),
              std::pair<Cipher, Mode>(Cipher::aes192, Mode::ecb) },
            { ::EVP_aes_192_cfb(),
              std::pair<Cipher, Mode>(Cipher::aes192, Mode::cfb) },
            { ::EVP_aes_192_ofb(),
              std::pair<Cipher, Mode>(Cipher::aes192, Mode::ofb) },
            // aes256
            { ::EVP_aes_256_cbc(),
              std::pair<Cipher, Mode>(Cipher::aes256, Mode::cbc) },
            { ::EVP_aes_256_ecb(),
              std::pair<Cipher, Mode>(Cipher::aes256, Mode::ecb) },
            { ::EVP_aes_256_cfb(),
              std::pair<Cipher, Mode>(Cipher::aes256, Mode::cfb) },
            { ::EVP_aes_256_ofb(),
              std::pair<Cipher, Mode>(Cipher::aes256, Mode::ofb) }
          };

        auto it = functions.find(function);
        if (it == functions.end())
          throw Error(elle::sprintf("unknown function '%s'", function));
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
    // Cipher
    uint8_t
    Serialize<elle::cryptography::Cipher>::convert(
      elle::cryptography::Cipher const& value)
    {
      return static_cast<uint8_t>(value);
    }

    elle::cryptography::Cipher
    Serialize<elle::cryptography::Cipher>::convert(
      uint8_t const& representation)
    {
      return static_cast<elle::cryptography::Cipher>(representation);
    }

    // Mode
    uint8_t
    Serialize<elle::cryptography::Mode>::convert(
      elle::cryptography::Mode const& value)
    {
      return static_cast<uint8_t>(value);
    }

    elle::cryptography::Mode
    Serialize<elle::cryptography::Mode>::convert(
      uint8_t const& representation)
    {
      return static_cast<elle::cryptography::Mode>(representation);
    }
  }
}
