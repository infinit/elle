#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <thread>
#include <mutex>

#include <elle/log.hh>

#include <cryptography/cryptography.hh>
#include <cryptography/random.hh>
#include <cryptography/Error.hh>

#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
# include <dopenssl/rand.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.cryptography");
#endif

namespace infinit
{
  namespace cryptography
  {
    static
    std::unordered_map<int, std::unique_ptr<std::mutex>>&
    mutexes()
    {
      static std::unordered_map<int, std::unique_ptr<std::mutex>> _mutexes;
      return _mutexes;
    }

    static
    std::mutex&
    crypto_lock_mutex()
    {
      static std::mutex _crypto_lock_mutex;
      return _crypto_lock_mutex;
    }

    static
    void
    crypto_threadid(CRYPTO_THREADID* id)
    {
      CRYPTO_THREADID_set_numeric(id,
        std::hash<std::thread::id>()(std::this_thread::get_id()));
    }

    static
    void
    crypto_lock(int mode, int n, const char* file, int line)
    {
      bool set = mode & CRYPTO_LOCK;
      std::mutex* mutex;
      {
        std::lock_guard<std::mutex> lock(crypto_lock_mutex());
        auto it = mutexes().find(n);
        if (it == mutexes().end())
          it = mutexes().emplace(n, std::make_unique<std::mutex>()).first;
        mutex = it->second.get();
      }
      if (set)
        mutex->lock();
      else
        mutex->unlock();
    }

    /*--------.
    | Classes |
    `--------*/

    // The cryptography initializer.
    class Initializer
    {
    public:
      Initializer()
      {
        // Ensure object depends on static variables to avoid static
        // initialization fiasco.
        mutexes();
        crypto_lock_mutex();

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
          throw Error(
            elle::sprintf("unable to initialize the deterministic PNRG "
                          "engine: %s",
                          ::ERR_error_string(ERR_get_error(), nullptr)));
#endif

        // Set thread-safeness procedures
        CRYPTO_set_locking_callback(crypto_lock);
        CRYPTO_THREADID_set_callback(crypto_threadid);
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
