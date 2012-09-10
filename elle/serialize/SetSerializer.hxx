#ifndef ELLE_SERIALIZE_SETSERIALIZER_HXX
# define ELLE_SERIALIZE_SETSERIALIZER_HXX

# include <unordered_set>
# include <set>

# include "detail/AssociativeSerializer.hh"

namespace elle { namespace serialize {

    template<typename T, typename Compare, typename Allocator>
      struct Serializer<std::set<T, Compare, Allocator>>
        : public detail::AssociativeSerializer<std::set<T, Compare, Allocator>>
      {};

    template<typename T, typename Compare, typename Allocator>
      struct StoreClassVersion<std::set<T, Compare, Allocator>>
      {
        static bool const value = false;
      };

// XXX set::unordered_set is a typedef of std::set on gcc, need to check
// on other os.
#if 0
    template<typename T, typename Compare, typename Allocator>
      struct Serializer<std::unordered_set<T, Compare, Allocator>>
        : public detail::AssociativeSerializer<std::unordered_set<T, Compare, Allocator>>
      {};

    template<typename T, typename Compare, typename Allocator>
      struct StoreClassVersion<std::unordered_set<T, Compare, Allocator>>
      {
        static bool const value = false;
      };
#endif

}}

#endif

