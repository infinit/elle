#ifndef ELLE_CRYPTOGRAPHY_RANDOM_HH
# define ELLE_CRYPTOGRAPHY_RANDOM_HH

# include <elle/types.hh>

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

      /// Generate a random value. The following versions are provided:
      ///
      ///   generate<bool>();
      ///   generate<char>();
      ///   generate<char>(char min, char max);
      ///   generate<double>();
      ///   generate<int8_t>();
      ///   generate<int8_t>(int8_t min, int8_t max);
      ///   generate<int16_t>();
      ///   generate<int16_t>(int16_t min, int16_t max);
      ///   generate<int32_t>();
      ///   generate<int32_t>(int32_t min, int32_t max);
      ///   generate<int64_t>();
      ///   generate<int64_t>(int64_t min, int64_t max);
      ///   generate<uint8_t>();
      ///   generate<uint8_t>(uint8_t min, uint8_t max);
      ///   generate<uint16_t>();
      ///   generate<uint16_t>(uint16_t min, uint16_t max);
      ///   generate<uint32_t>();
      ///   generate<uint32_t>(uint32_t min, uint32_t max);
      ///   generate<uint64_t>();
      ///   generate<uint64_t>(uint64_t min, uint64_t max);
      template <typename T>
      T
      generate();

      template <typename T>
      T
      generate(T min, T max);

      /// generate<std::string>(uint32_t length);
      /// generate<elle::Buffer>(uint32_t size);
      template <typename T>
      T
      generate(uint32_t const length);

      /// Fill a buffer with random bytes. See RAND_bytes.
      void
      fill(unsigned char* buffer,
           size_t const size);

      /// Set up the random generator by adding entropy.
      ///
      /// !WARNING! Do not use unless you know exactly what you're doing.
      void
      setup();
    }
  }
}

# include <cryptography/random.hxx>

#endif
