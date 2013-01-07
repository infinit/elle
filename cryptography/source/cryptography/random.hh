#ifndef INFINIT_CRYPTOGRAPHY_RANDOM_HH
# define INFINIT_CRYPTOGRAPHY_RANDOM_HH

# include <elle/types.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Provide functionalities for generating random values.
    namespace random
    {
      /*----------.
      | Functions |
      `----------*/
      /// Set up the random generator by adding entropy.
      ///
      /// !WARNING! Do not use unless you know exactly what you're doing.
      void
      setup();
      /// Generate a random value for the given type.
      template <typename T>
      T
      generate();
      /// Generate a random value based on another metric.
      template <typename T>
      T
      generate(elle::Natural32 const metric);
    }
  }
}

# include <cryptography/random.hxx>

#endif
