#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined INFINIT_WINDOWS
# include <process.h>
# include <elle/windows.hh>
# include <wincrypt.h>
#elif defined INFINIT_IOS
# include <Security/Security.h>
#endif

#include <system_error>
#include <iostream>
#include <fstream>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <elle/Buffer.hh>
#include <elle/cryptography/Error.hh>
#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/random.hh>
#include <elle/log.hh>
#include <elle/os/environ.hh>
#include <elle/system/platform.hh>

ELLE_LOG_COMPONENT("elle.cryptography.random");

namespace elle
{
  namespace cryptography
  {
    namespace random
    {
      /*----------.
      | Functions |
      `----------*/

      void
      setup()
      {
        uint8_t temporary[256];
        ELLE_TRACE("setting up the random generator")
#if defined INFINIT_LINUX || defined INFINIT_MACOSX || defined INFINIT_ANDROID
        {
          /// The path to read random data from.
          ///
          /// Enable for instance tests to override the random data source,
          /// so as to use /dev/urandom and avoid /dev/random enthropy
          /// starvation.

          static std::string const source =
            elle::os::getenv("ELLE_CRYPTOGRAPHY_RANDOM_SOURCE",
                             "/dev/urandom");

          std::ifstream random_source_file(source);

          if (random_source_file.good() == false)
            throw Error(
              elle::sprintf("unable to open the random source file '%s': %s",
                            source,
                            std::error_code(errno,
                                            std::system_category()).message()));

          // Read random data.
          random_source_file.read(reinterpret_cast<char *>(temporary),
                                  sizeof (temporary));
        }
#elif defined INFINIT_IOS
        {
          int res = SecRandomCopyBytes(kSecRandomDefault, 256, temporary);
          if (res != 0)
          {
            throw Error("unable to get 256 bytes of random data");
          }
        }
#elif defined INFINIT_WINDOWS
        {
          HCRYPTPROV h_provider = 0;

          if (!::CryptAcquireContextW(&h_provider, 0, 0, PROV_RSA_FULL,
                                      CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
            throw Error("unable to acquire a cryptographic context");

          if (!::CryptGenRandom(h_provider, sizeof (temporary), temporary))
          {
            ::CryptReleaseContext(h_provider, 0);

            throw Error("unable to generate random bytes");
          }

          if (!::CryptReleaseContext(h_provider, 0))
            throw Error("failed to release cryptographic context");
        }
#else
# error "unsupported platform"
#endif

        // Seed the random generator.
        ::RAND_seed(temporary, sizeof (temporary));
      }

      void
      fill(unsigned char *buffer,
           size_t const size)
      {
        if (::RAND_bytes(buffer, size) <= 0)
          throw Error(elle::sprintf("unable to generate random bytes: %s",
                                    ::ERR_error_string(ERR_get_error(),
                                                       nullptr)));
      }

      /*----------.
      | Functions |
      `----------*/

      /// Val is in [T::min, T::max].  Bring it into [min, max].
      template <typename T>
      T
      _rangify(T const Val,
               T const min, T const max)
      {
        ELLE_ASSERT_LTE(min, max);
        auto tmin = double(std::numeric_limits<T>::min());
        auto tmax = double(std::numeric_limits<T>::max());
        // The width of [T::min, T::max].
        auto twidth = tmax - tmin;
        // The width of [min, max].
        auto width = double(max) - double(min);
        return min + static_cast<T>((double(Val) - tmin) / twidth * width);
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
      T
      generate()
      {
        return _generate<T>();
      }

      template <typename T>
      T
      generate(T min, T max)
      {
        return _rangify(generate<T>(), min, max);
      }

      template <>
      bool
      generate<bool>()
      {
        // Because of RVO, we actually get a "bool" which ranges
        // from 0 to 255 here.
        return _generate<bool>() % 2;
      }

#define ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(Type)        \
      template Type generate<Type>();                   \
      template Type generate<Type>(Type min, Type max)

      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(char);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(int8_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(int16_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(int32_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(int64_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(uint8_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(uint16_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(uint32_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(uint64_t);
      ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR(double);
#undef ELLE_CRYPTOGRAPHY_RANDOM_GENERATOR

      template <typename T>
      T
      generate(uint32_t const length);

      template <>
      std::string
      generate<std::string>(uint32_t const length)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();

        static std::string const alphabet =
          "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "0123456789@#$%^&*)_+=-\';:|.,<>?`~";

        auto res = std::string{};
        res.resize(length);
        for (uint32_t i = 0; i < length; i++)
          res[i] = alphabet[generate<int>(0, alphabet.length())];
        return res;
      }

      template <>
      elle::Buffer
      generate<elle::Buffer>(uint32_t const size)
      {
        // Make sure the cryptographic system is set up.
        cryptography::require();
        auto res = elle::Buffer(int(size));
        fill(reinterpret_cast<unsigned char*>(res.mutable_contents()),
             res.size());
        return res;
      };
    }
  }
}
