#include <cryptography/cryptography.hh>
#include <cryptography/random.hh>
# include <cryptography/Exception.hh>

#include <elle/log.hh>

#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rand.hh>
#endif

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
ELLE_LOG_COMPONENT("infinit.cryptography.cryptography");
#endif

namespace infinit
{
  namespace cryptography
  {
    /*--------.
    | Classes |
    `--------*/

    // The cryptography initializer.
    class Initializer
    {
    public:
      Initializer()
      {
        // Load the crypto error strings.
        ::ERR_load_crypto_strings();

        // Enable the SSL algorithms, especially for RSA.
        ::SSLeay_add_all_algorithms();

        // Setup the random generator so as to generate more entropy if
        // required.
        if (::RAND_status() == 0)
          random::setup();

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // Initialize the deterministic PNRG engine.
        if (::dRAND_init() != 1)
          throw Exception(
            elle::sprintf("unable to initialize the deterministic PNRG "
                          "engine: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
#endif
      }

      ~Initializer()
      {
#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        // Clean the deterministic PNRG engine.
        if (::dRAND_clean() != 1)
        {
          ELLE_ERR("unable to clean the deterministic PNRG "
                   "engine: %s",
                   ::ERR_error_string(ERR_get_error(), nullptr));
        }
#endif

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
      }
    };

    /*----------.
    | Functions |
    `----------*/

    void
    require()
    {
      static Initializer initialized;
    }
  }
}
