#ifndef ELLE_SERIALIZE_ARRAYSERIALIZER_HXX
# define ELLE_SERIALIZE_ARRAYSERIALIZER_HXX

# include <array>

# include "Serializer.hh"

///
/// This file provides Serializer specializations
/// in order to serialize static arrays for both C type (`T[size]')
/// and C++11 array class (`std::array<T, size>').
///

namespace elle { namespace serialize {

    namespace detail {

        template<typename ArrayType, size_t size>
          struct ArraySerializer
          {
            template<typename Archive>
              static inline void Serialize(Archive& ar, ArrayType& array, unsigned int)
              {
                // We could inline everything here (we statically know the size),
                // but I think the compiler should know when it's best
                // to inline or not depending on the size value.
                for (size_t i = 0; i < size; ++i)
                  {
                    ar & array[i];
                  }
              }

          };

    } // namespace detail

    template<typename T, size_t size>
      struct Serializer<T[size]>
        : public detail::ArraySerializer<T[size], size>
      {};

    template<typename T, size_t size>
      struct StoreClassVersion<T[size]>
      {
        static bool const value = false;
      };

    template<typename T, size_t size>
      struct Serializer<std::array<T, size>>
        : public detail::ArraySerializer<std::array<T, size>, size>
      {};

    template<typename T, size_t size>
      struct StoreClassVersion<std::array<T, size>>
      {
        static bool const value = false;
      };

}} // !namespace elle::serialize


#endif
