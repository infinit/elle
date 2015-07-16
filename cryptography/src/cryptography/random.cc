#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(INFINIT_WINDOWS)
# include <process.h>
# include <windows.h>
# include <wincrypt.h>
#elif defined(INFINIT_IOS)
# include <Security/Security.h>
#endif

#include <system_error>
#include <iostream>
#include <fstream>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <elle/system/platform.hh>
#include <elle/os/environ.hh>
#include <elle/log.hh>

#include <cryptography/random.hh>
#include <cryptography/Error.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.random");

namespace infinit
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

        ELLE_TRACE("setting up the random generator");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX) || defined(INFINIT_ANDROID)
        {
          /// The path to read random data from.
          ///
          /// Enable for instance tests to override the random data source,
          /// so as to use /dev/urandom and avoid /dev/random enthropy
          /// starvation.

          static std::string const source =
            elle::os::getenv("INFINIT_CRYPTOGRAPHY_RANDOM_SOURCE",
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
#elif defined(INFINIT_IOS)
        {
          int res = SecRandomCopyBytes(kSecRandomDefault, 256, temporary);
          if (res != 0)
          {
            throw Error("unable to get 256 bytes of random data");
          }
        }
#elif defined(INFINIT_WINDOWS)
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
    }
  }
}
