#ifndef INFINIT_CRYPTOGRAPHY_RANDOM_HXX
# define INFINIT_CRYPTOGRAPHY_RANDOM_HXX

# include <elle/Exception.hh>
# include <elle/Buffer.hh>

# include <cryptography/cryptography.hh>

# include <openssl/engine.h>
# include <openssl/rand.h>
# include <openssl/err.h>

namespace infinit
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
        struct Generator<elle::Boolean>
        {
          static
          elle::Boolean
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Integer32 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            if (value > 0)
              return (true);
            else
              return (false);
          }
        };

        template <>
        struct Generator<elle::Character>
        {
          static
          elle::Character
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Character value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Real>
        {
          static
          elle::Real
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Real value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Integer8>
        {
          static
          elle::Integer8
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Integer8 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Integer16>
        {
          static
          elle::Integer16
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Integer16 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Integer32>
        {
          static
          elle::Integer32
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Integer32 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Integer64>
        {
          static
          elle::Integer64
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Integer64 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Natural8>
        {
          static
          elle::Natural8
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Natural8 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Natural16>
        {
          static
          elle::Natural16
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Natural16 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Natural32>
        {
          static
          elle::Natural32
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Natural32 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::Natural64>
        {
          static
          elle::Natural64
          generate()
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Natural64 value;

            if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                             sizeof (value)) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

            return (value);
          }
        };

        template <>
        struct Generator<elle::String>
        {
          static
          elle::String
          generate(elle::Natural32 const length)
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            static elle::String alphabet =
              "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
              "0123456789@#$%^&*)_+=-\';:|.,<>?`~";
            elle::Natural32 i;
            elle::String value;

            // Allocate the string.
            value.resize(length);

            // Generate the characters.
            for (i = 0; i < length; i++)
              {
                unsigned char c;

                if (::RAND_bytes(&c, 1) == 0)
                  throw elle::Exception("%s",
                                        ::ERR_error_string(ERR_get_error(),
                                                           nullptr));

                value[i] = alphabet[c % alphabet.length()];
              }

            return (value);
          }
        };

        template <>
        struct Generator<elle::Buffer>
        {
          static
          elle::Buffer
          generate(elle::Natural32 const size)
          {
            // Make sure the cryptographic system is set up.
            cryptography::require();

            elle::Buffer value(size);

            // generate a random integer.
            if (::RAND_bytes(
                  reinterpret_cast<unsigned char*>(value.mutable_contents()),
                  value.size()) == 0)
              throw elle::Exception("%s", ::ERR_error_string(ERR_get_error(),
                                                             nullptr));

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
      generate(elle::Natural32 const metric)
      {
        return (Generator<T>::generate(metric));
      }
    }
  }
}

#endif
