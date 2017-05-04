#include <elle/random.hh>

#ifdef VALGRIND
# include <valgrind/valgrind.h>
#else
# define RUNNING_ON_VALGRIND 0
#endif

#include <elle/os/environ.hh>

namespace elle
{
  /// A unique random device.
  std::mt19937&
  random_engine()
  {
    static auto res = []
      {
        // Valgrind does not like the random_device.
        // http://stackoverflow.com/questions/37032339.
        if (os::getenv("ELLE_SEED", false)
            || RUNNING_ON_VALGRIND)
          return std::mt19937{std::mt19937::default_seed};
        else
        {
          std::random_device rd{};
          return std::mt19937{rd()};
        }
      }();
    return res;
  }
}
