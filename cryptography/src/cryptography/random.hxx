#ifndef INFINIT_CRYPTOGRAPHY_RANDOM_HXX
# define INFINIT_CRYPTOGRAPHY_RANDOM_HXX

# include <cstdlib>

# include <cryptography/cryptography.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>
# include <elle/finally.hh>

# include <openssl/engine.h>
# include <openssl/rand.h>
# include <openssl/err.h>

/*----------------.
| Macro-functions |
`----------------*/

# define INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(_type_)                  \
  template <>                                                           \
  struct Generator<_type_>                                              \
  {                                                                     \
    static                                                              \
    _type_                                                              \
    generate()                                                          \
    {                                                                   \
      return _generate<_type_>();                                       \
    }                                                                   \
                                                                        \
    static                                                              \
    _type_                                                              \
    generate(_type_ minimum,                                            \
             _type_ maximum)                                            \
    {                                                                   \
      return (::infinit::cryptography::random::_rangify(                \
                _generate<_type_>(), minimum, maximum));                \
    }                                                                   \
  };

namespace infinit
{
  namespace cryptography
  {
    namespace random
    {
      /*----------.
      | Functions |
      `----------*/

      template <typename T>
      typename std::enable_if<std::is_signed<T>::value, T>::type
      _abs(T const value)
      {
        return std::abs(value);
      }

      template <typename T>
      typename std::enable_if<std::is_unsigned<T>::value, T>::type
      _abs(T const value)
      {
        return value;
      }

      template <typename T>
      T
      _rangify(T const value,
               T const minimum,
               T const maximum)
      {
        ELLE_ASSERT_LTE(minimum, maximum);

        T ranged =
          static_cast<T>(
            static_cast<double>(_abs(value)) /
            (static_cast<double>(std::numeric_limits<T>::max()) -
             static_cast<double>(std::numeric_limits<T>::min())) *
            (static_cast<double>(maximum) -
             static_cast<double>(minimum)));

        return (minimum + ranged);
      }

      template <typename T>
      T
      _generate()
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
        T value;
        fill(reinterpret_cast<unsigned char*>(&value), sizeof value);
        return value;
      }

      /*-----------.
      | Generators |
      `-----------*/

      template <typename T>
      struct Generator
      {
        static
        T
        generate()
        {
          static_assert(sizeof(T) == 0,
                        "unsupported type for random generation");
        }
      };

      template <>
      struct Generator<bool>
      {
        static
        bool
        generate()
        {
          int32_t value = _generate<bool>();
          return value > 0;
        }
      };

      template <>
      struct Generator<double>
      {
        static
        double
        generate()
        {
          return _generate<double>();
        }
      };

      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(char);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(int8_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(int16_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(int32_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(int64_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(uint8_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(uint16_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(uint32_t);
      INFINIT_CRYPTOGRAPHY_RANDOM_GENERATOR(uint64_t);

      template <>
      struct Generator<std::string>
      {
        static
        std::string
        generate(uint32_t const length)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          static std::string const alphabet =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789@#$%^&*)_+=-\';:|.,<>?`~";

          std::string value;

          value.resize(length);

          for (uint32_t i = 0; i < length; i++)
          {
            uint32_t n;
            fill(reinterpret_cast<unsigned char*>(&n), sizeof (n));
            value[i] = alphabet[n % alphabet.length()];
          }

          return (value);
        }
      };

      template <>
      struct Generator<elle::Buffer>
      {
        static
        elle::Buffer
        generate(uint32_t const size)
        {
          // Make sure the cryptographic system is set up.
          cryptography::require();

          elle::Buffer value(static_cast<int>(size));

          fill(
            reinterpret_cast<unsigned char*>(value.mutable_contents()),
            value.size());

          return (value);
        }
      };

      /*----------.
      | Functions |
      `----------*/

      template <typename T,
                typename... A>
      T
      generate(A... arguments)
      {
        return (Generator<T>::generate(arguments...));
      }
    }
  }
}

#endif
