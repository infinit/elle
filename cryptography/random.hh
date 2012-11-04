#ifndef ELLE_CRYPTOGRAPHY_RANDOM_HH
# define ELLE_CRYPTOGRAPHY_RANDOM_HH

# include <elle/types.hh>

# include <elle/standalone/fwd.hh>

namespace elle
{
  namespace cryptography
  {
    /// Provide functionalities for generating random values.
    namespace random
    {
      /*----------.
      | Functions |
      `----------*/
      /// Initialize the random generator.
      void
      initialize();
      /// Clean the random generator.
      void
      clean();
      /// Generate a random value for the given type.
      template <typename T>
      T
      generate();
      /// Generate a random value based on another metric.
      template <typename T>
      T
      generate(Natural32 const metric);
    }
  }
}

# include <elle/cryptography/random.hxx>

#endif
