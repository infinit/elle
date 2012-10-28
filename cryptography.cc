#include <elle/cryptography/cryptography.hh>
#include <elle/cryptography/Random.hh>
#include <elle/cryptography/KeyPair.hh>

#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/err.h>

ELLE_LOG_COMPONENT("elle.cryptography");

namespace elle
{
  namespace cryptography
  {

    /*----------.
    | Functions |
    `----------*/

    static elle::Boolean _initialized = false;

    void
    initialize()
    {
      ELLE_TRACE_SCOPE("initializing the cryptography");

      // Load the crypto error strings.
      ::ERR_load_crypto_strings();

      // Enable the SSL algorithms, especially for RSA.
      ::SSLeay_add_all_algorithms();

      // Initialize the random and keypair classes.
      Random::initialize();
      KeyPair::initialize();

      // Set the module has initialized.
      _initialized = true;

      ELLE_TRACE_SCOPE("cryptography initialized");
    }

    void
    clean()
    {
      ELLE_TRACE_SCOPE("cleaning the cryptography");

      // Clean the keypair and random classes.
      KeyPair::clean();
      Random::clean();

      // Free the current threads error queue.
      ::ERR_remove_state(0);

      // Clean the engine.
      ::ENGINE_cleanup();

      // Free the error strings.
      ::ERR_free_strings();

      // Clean the evp environment.
      ::EVP_cleanup();

      // Release the extra data.
      ::CRYPTO_cleanup_all_ex_data();

      // Set the module has non-initialized.
      _initialized = false;

      ELLE_TRACE_SCOPE("cryptography cleaned");
    }

    void
    require()
    {
      ELLE_DEBUG_SCOPE("require the cryptography");

      if (_initialized == false)
        initialize();
    }

  }
}
