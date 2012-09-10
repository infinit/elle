#ifndef ELLE_SERIALIZE_LISTSERIALIZER_HXX
# define ELLE_SERIALIZE_LISTSERIALIZER_HXX

# include <list>

# include "detail/SequenceSerializer.hh"
# include "Serializer.hh"

namespace elle { namespace serialize {

    template<typename T, typename Allocator>
      struct Serializer<std::list<T, Allocator>>
        : public detail::SequenceSerializer<std::list<T, Allocator>>
      {};

    template<typename T, typename Allocator>
      struct StoreClassVersion<std::list<T, Allocator>>
      {
        static bool const value = false;
      };

}}

#endif
