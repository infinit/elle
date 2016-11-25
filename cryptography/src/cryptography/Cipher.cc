#include <elle/log.hh>

#include <cryptography/Cipher.hh>
#include <cryptography/Error.hh>

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
          throw Error(elle::sprintf("unknown cipher '%s'",
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
          throw Error(elle::sprintf("unknown operation mode '%s'",
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
        static std::map< ::EVP_CIPHER const*,
                         std::pair<Cipher, Mode> > const functions =
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

        for (auto const& iterator: functions)
        {
          if (function == iterator.first)
            return (iterator.second);
        }

        throw Error(elle::sprintf("unknown function '%s'", function));
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
    Serialize<infinit::cryptography::Cipher>::convert(
      infinit::cryptography::Cipher const& value)
    {
      return (static_cast<uint8_t>(value));
    }

    infinit::cryptography::Cipher
    Serialize<infinit::cryptography::Cipher>::convert(
      uint8_t const& representation)
    {
      return (static_cast<infinit::cryptography::Cipher>(representation));
    }

    // Mode
    uint8_t
    Serialize<infinit::cryptography::Mode>::convert(
      infinit::cryptography::Mode const& value)
    {
      return (static_cast<uint8_t>(value));
    }

    infinit::cryptography::Mode
    Serialize<infinit::cryptography::Mode>::convert(
      uint8_t const& representation)
    {
      return (static_cast<infinit::cryptography::Mode>(representation));
    }
  }
}
