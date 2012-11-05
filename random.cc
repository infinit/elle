#include <elle/system/platform.hh>

#include <elle/cryptography/random.hh>

#include <elle/log.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/rand.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# if defined(INFINIT_WINDOWS)
#  include <process.h>
#  include <windows.h>
#  include <wincrypt.h>
# endif
#include <elle/idiom/Open.hh>

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
      initialize()
      {
        uint8_t temporary[256];

        ELLE_TRACE("initializing the random generator");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
        {
          static const char* source;
          int fd = -1;

          /// The path to read random data from.
          ///
          /// Enable for instance tests to override the random data source,
          /// so as to use /dev/urandom and avoid /dev/random enthropy
          /// starvation.
          if ((source = getenv("ELLE_RANDOM_SOURCE")) == nullptr)
            source = "/dev/random";

          if ((fd = ::open(source, O_RDONLY)) == -1)
            escape("%s", ::strerror(errno));

          // Read random data.
          if (::read(fd, temporary, sizeof (temporary)) == -1)
            {
              ::close(fd);

              escape("%s", ::strerror(errno));
            }

          // Close the file descriptor.
          ::close(fd);
        }
#elif defined(INFINIT_WINDOWS)
        {
          HCRYPTPROV        h_provider = 0;

          if (!::CryptAcquireContextW(&h_provider, 0, 0, PROV_RSA_FULL,
                                      CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
            escape("failed to acquire cryptographic context");

          if (!::CryptGenRandom(h_provider, sizeof (temporary), temporary))
            {
              ::CryptReleaseContext(h_provider, 0);

              escape("failed to generate the random seed");
            }

          if (!::CryptReleaseContext(h_provider, 0))
            escape("failed to release cryptographic context");
        }
#else
# error "unsupported platform"
#endif

        // Seed the random generator.
        ::RAND_seed(temporary, sizeof (temporary));
      }

      void
      clean()
      {
        ELLE_TRACE("cleaning the random generator");
      }
    }
  }
}
