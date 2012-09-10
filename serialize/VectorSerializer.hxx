#ifndef ELLE_SERIALIZE_VECTORSERIALIZER_HXX
# define ELLE_SERIALIZE_VECTORSERIALIZER_HXX

# include <vector>

# include "detail/SequenceSerializer.hh"
# include "Serializer.hh"

namespace elle { namespace serialize {

    template<typename T, typename Allocator>
      struct Serializer<std::vector<T, Allocator>>
        : public detail::SequenceSerializer<std::vector<T, Allocator>>
      {};

    template<typename T, typename Allocator>
      struct StoreFormat<std::vector<T, Allocator>>
      {
        static bool const value = false;
      };

}}

#endif

