#ifndef ELLE_RANDOM_HXX
# define ELLE_RANDOM_HXX

# include <elle/Exception.hh>
# include <elle/Buffer.hh>

# include <elle/cryptography/cryptography.hh>

# include <openssl/engine.h>
# include <openssl/rand.h>
# include <openssl/err.h>

namespace elle
{
  namespace cryptography
  {
    namespace random
    {
      /// Contain all the Generator structure specializations.
      namespace
      {
        template <typename T>
        struct Generator
        {
          static
          T
          generate()
          {
            static_assert(sizeof (T) == 0,
                          "unsupported type for random generation");
          }
        };

        template <>
        struct Generator<Boolean>
        {
          static
          Boolean
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Integer32 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            if (value > 0)
              return (true);
            else
              return (false);
          }
        };

        template <>
        struct Generator<Character>
        {
          static
          Character
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Character value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Real>
        {
          static
          Real
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Real value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Integer8>
        {
          static
          Integer8
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Integer8 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Integer16>
        {
          static
          Integer16
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Integer16 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Integer32>
        {
          static
          Integer32
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Integer32 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Integer64>
        {
          static
          Integer64
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Integer64 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Natural8>
        {
          static
          Natural8
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Natural8 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Natural16>
        {
          static
          Natural16
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Natural16 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Natural32>
        {
          static
          Natural32
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Natural32 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<Natural64>
        {
          static
          Natural64
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Natural64 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<String>
        {
          static
          String
          generate(Natural32 const length)
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            static String alphabet =
              "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
              "0123456789@#$%^&*)_+=-\';:|.,<>?`~";
            Natural32 i;
            String value;

            // Allocate the string.
            value.resize(length);

            // Generate the characters.
            for (i = 0; i < length; i++)
              {
                unsigned char c;

                if (::RAND_bytes(&c, 1) == 0)
                  throw Exception("%s",
                                  ::ERR_error_string(ERR_get_error(), nullptr));

                value[i] = alphabet[c % alphabet.length()];
              }

            return (value);
          }
        };

        template <>
        struct Generator<Buffer>
        {
          static
          Buffer
          generate(Natural32 const size)
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            Buffer value(size);

            // generate a random integer.
            if (::RAND_bytes(
                  reinterpret_cast<unsigned char*>(value.mutable_contents()),
                  value.size()) == 0)
              throw Exception("%s",
                              ::ERR_error_string(ERR_get_error(), nullptr));

            return (value);
          }
        };
      }

      template <typename T>
      T
      generate()
      {
        return (Generator<T>::generate());
      }

      template <typename T>
      T
      generate(Natural32 const metric)
      {
        return (Generator<T>::generate(metric));
      }
    }
  }
}

#endif
