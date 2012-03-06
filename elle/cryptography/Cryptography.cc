//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 12:16:42 2007]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Cryptography.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises everything related to the cryptographic
    /// classes.
    ///
    Status              Cryptography::Initialize()
    {
      // load the crypto error strings.
      ::ERR_load_crypto_strings();

      // enable the SSL algorithms, especially for RSA.
      ::SSLeay_add_all_algorithms();

      // initialize the random generator.
      if (Random::Initialize() == StatusError)
        escape("unable to initialize the random generator");

      // initialize the key pair generation context.
      if (KeyPair::Initialize() == StatusError)
        escape("unable to initialize the key pair generation context");

      return StatusOk;
    }

    ///
    /// this method cleans static cryptographic resources.
    ///
    Status              Cryptography::Clean()
    {
      // clean the key pair generation context.
      if (KeyPair::Clean() == StatusError)
        escape("unable to initialize the key pair generation context");

      // clean the random generator.
      if (Random::Clean() == StatusError)
        escape("unable to clean the random generator");

      // free the current threads error queue.
      ::ERR_remove_state(0);

      // clean the engine.
      ::ENGINE_cleanup();

      // free the error strings.
      ::ERR_free_strings();

      // clean the evp environment.
      ::EVP_cleanup();

      // release the extra data.
      ::CRYPTO_cleanup_all_ex_data();

      return StatusOk;
    }

  }
}
