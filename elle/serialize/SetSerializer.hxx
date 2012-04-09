#ifndef ELLE_SERIALIZE_SETSERIALIZER_HXX
# define ELLE_SERIALIZE_SETSERIALIZER_HXX

# include <set>

# include "detail/AssociativeSerializer.hh"

namespace elle { namespace serialize {

    template<typename T, typename Compare, typename Allocator>
      struct ArchiveSerializer<std::set<T, Compare, Allocator>>
        : public detail::AssociativeSerializer<std::set<T, Compare, Allocator>>
      {};

    template<typename T, typename Compare, typename Allocator>
      struct StoreClassVersion<std::set<T, Compare, Allocator>>
      {
        static bool const value = false;
      };

}}

#endif

