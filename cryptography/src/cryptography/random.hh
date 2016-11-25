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

      /// Generate a random value. The following versions are provided:
      ///
      ///   generate<bool>();
      ///   generate<char>();
      ///   generate<char>(char minimum,
      ///                  char maximum);
      ///   generate<double>();
      ///   generate<int8_t>();
      ///   generate<int8_t>(int8_t minimum,
      ///                    int8_t maximum);
      ///   generate<int16_t>();
      ///   generate<int16_t>(int16_t minimum,
      ///                     int16_t maximum);
      ///   generate<int32_t>();
      ///   generate<int32_t>(int32_t minimum,
      ///                     int32_t maximum);
      ///   generate<int64_t>();
      ///   generate<int64_t>(int64_t minimum,
      ///                     int64_t maximum);
      ///   generate<uint8_t>();
      ///   generate<uint8_t>(uint8_t minimum,
      ///                     uint8_t maximum);
      ///   generate<uint16_t>();
      ///   generate<uint16_t>(uint16_t minimum,
      ///                      uint16_t maximum);
      ///   generate<uint32_t>();
      ///   generate<uint32_t>(uint32_t minimum,
      ///                      uint32_t maximum);
      ///   generate<uint64_t>();
      ///   generate<uint64_t>(uint64_t minimum,
      ///                      uint64_t maximum);
      ///   generate<std::string>(uint32_t length);
      ///   generate<elle::Buffer>(uint32_t size);
      template <typename T,
                typename... A>
      T
      generate(A... arguments);
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
