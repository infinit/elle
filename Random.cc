#include <elle/system/platform.hh>

#include <elle/cryptography/Random.hh>

#include <elle/standalone/Region.hh>

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

namespace elle
{
  namespace cryptography
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the random system.
    ///
    Status              Random::Initialize()
    {
      uint8_t           temporary[256];

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      {
        int             fd = -1;

        // get some random data.
        static const char* source;

        /// The path to read random data from.
        ///
        /// Enable for instance tests to override the random data source,
        /// so as to use /dev/urandom and avoid /dev/random enthropy
        /// starvation.
        if ((source = getenv("INFINIT_RANDOM_SOURCE")) == nullptr)
          source = "/dev/random";

        if ((fd = ::open(source, O_RDONLY)) == -1)
          escape("%s", ::strerror(errno));

        // read random data.
        if (::read(fd, temporary, sizeof (temporary)) == -1)
          {
            ::close(fd);

            escape("%s", ::strerror(errno));
          }

        // close the file descriptor.
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

      // seed the random generator.
      ::RAND_seed(temporary, sizeof (temporary));

      return Status::Ok;
    }

    ///
    /// this method cleans the system.
    ///
    Status              Random::Clean()
    {
      // nothing to do.

      return Status::Ok;
    }

    ///
    /// this method generates a boolean.
    ///
    Status              Random::Generate(Boolean&               value)
    {
      elle::Integer32   r;

      // generate a random integer.
      if (Random::Generate(r) == Status::Error)
        escape("unable to generate a random integer");

      // set the boolean.
      if (r > 0)
        value = true;
      else
        value = false;

      return Status::Ok;
    }

    ///
    /// this method generates a character.
    ///
    Status              Random::Generate(Character&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a real.
    ///
    Status              Random::Generate(Real&                  value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates an integer.
    ///
    Status              Random::Generate(Integer8&              value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates an integer.
    ///
    Status              Random::Generate(Integer16&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates an integer.
    ///
    Status              Random::Generate(Integer32&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates an integer.
    ///
    Status              Random::Generate(Integer64&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a natural.
    ///
    Status              Random::Generate(Natural8&              value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a natural.
    ///
    Status              Random::Generate(Natural16&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a natural.
    ///
    Status              Random::Generate(Natural32&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a natural.
    ///
    Status              Random::Generate(Natural64&             value)
    {
      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
                       sizeof (value)) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a large.
    ///
    Status              Random::Generate(Large&                 value,
                                         const Natural32        length)
    {
      // generate a random BN.
      if (::BN_rand(&value, length, -1, 0) == 0)
        escape("%s", ::ERR_error_string(ERR_get_error(), nullptr));

      return Status::Ok;
    }

    ///
    /// this method generates a string.
    ///
    Status              Random::Generate(String&                value,
                                         const Natural32        length)
    {
      static String     alphabet =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789@#$%^&*)_+=-\';:|.,<>?`~";
      Natural32         i;

      // resize the string.
      value.resize(length);

      // generate characters.
      for (i = 0; i < length; i++)
        {
          unsigned char c;

          ::RAND_bytes(&c, 1);

          value[i] = alphabet[c % alphabet.length()];
        }

      return Status::Ok;
    }

    ///
    /// this method generates a region.
    ///
    Status              Random::Generate(standalone::Region& value,
                                         const Natural32        size)
    {
      // prepare the region.
      if (value.Prepare(size) == Status::Error)
        escape("unable to prepare the region");

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(value.contents),
                       size) == 0)
        escape("%s", ::strerror(errno));

      // set the region's size.
      value.size = size;

      return Status::Ok;
    }

  }
}
