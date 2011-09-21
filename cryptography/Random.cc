//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu aug 11 16:15:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/cryptography/Random.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/rand.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# if INFINIT_WIN32
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
    Status		Random::Initialize()
    {
      uint8_t		temporary[256];

      enter();

      // initialise the random generator.
      ::srand(::time(NULL));

#if INFINIT_UNIX

      int		fd = -1;

      // get some random data.
      if ((fd = ::open("/dev/random", O_RDONLY)) == -1)
	escape(::strerror(errno));

      // read random data.
      if (::read(fd, temporary, sizeof (temporary)) == -1)
	{
	  ::close(fd);

	  escape(::strerror(errno));
	}

      // close the file descriptor.
      ::close(fd);

#elif INFINIT_WIN32

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

#else
# error "unsupported platform"
#endif

      // seed the random generator.
      ::RAND_seed(temporary, sizeof (temporary));

      leave();
    }

    ///
    /// this method cleans the system.
    ///
    Status		Random::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method generates a boolean.
    ///
    Status		Random::Generate(Boolean&		value)
    {
      elle::Integer32	r;

      enter();

      // generate a random integer.
      if (Random::Generate(r) == StatusError)
	escape("unable to generate a random integer");

      // set the boolean.
      if (r > 0)
	value = true;
      else
	value = false;

      leave();
    }

    ///
    /// this method generates a character.
    ///
    Status		Random::Generate(Character&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a real.
    ///
    Status		Random::Generate(Real&			value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates an integer.
    ///
    Status		Random::Generate(Integer8&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates an integer.
    ///
    Status		Random::Generate(Integer16&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates an integer.
    ///
    Status		Random::Generate(Integer32&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates an integer.
    ///
    Status		Random::Generate(Integer64&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a natural.
    ///
    Status		Random::Generate(Natural8&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a natural.
    ///
    Status		Random::Generate(Natural16&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a natural.
    ///
    Status		Random::Generate(Natural32&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a natural.
    ///
    Status		Random::Generate(Natural64&		value)
    {
      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&value),
		       sizeof (value)) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a large.
    ///
    Status		Random::Generate(Large&			value,
					 const Natural32	length)
    {
      enter();

      // generate a random BN.
      if (::BN_rand(&value, length, -1, 0) == 0)
	escape(::ERR_error_string(ERR_get_error(), NULL));

      leave();
    }

    ///
    /// this method generates a string.
    ///
    Status		Random::Generate(String&		value,
					 const Natural32	length)
    {
      char		buffer[length];

      enter();

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(&buffer),
		       sizeof (buffer)) == 0)
	escape(::strerror(errno));

      // assign the characters.
      value.assign(buffer, length);

      leave();
    }

    ///
    /// this method generates a region.
    ///
    Status		Random::Generate(Region&		value,
					 const Natural32	size)
    {
      enter();

      // prepare the region.
      if (value.Prepare(size) == StatusError)
	escape("unable to prepare the region");

      // generate a random integer.
      if (::RAND_bytes(reinterpret_cast<unsigned char*>(value.contents),
		       size) == 0)
	escape(::strerror(errno));

      // set the region's size.
      value.size = size;

      leave();
    }

  }
}
