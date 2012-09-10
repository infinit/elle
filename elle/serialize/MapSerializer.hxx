#ifndef ELLE_SERIALIZE_MAPSERIALIZER_HXX
# define ELLE_SERIALIZE_MAPSERIALIZER_HXX

# include <unordered_map>
# include <map>

# include "detail/SequenceSerializer.hh"

# include "PairSerializer.hxx"

namespace elle { namespace serialize {

    namespace stl_insert {

        template<typename Container>
          struct InsertPair
          {
            typedef typename Container::value_type::first_type First;
            typedef typename Container::value_type::second_type Second;
            typedef std::pair<
                typename std::remove_cv<First>::type
              , typename std::remove_cv<Second>::type
            > ValueType;

            static inline void Save(Container& container,
                                    ValueType const& value)
            { container.insert(value); }
          };

    } // !ns stl_insert

    template<typename Key, typename T, typename Compare, typename Allocator>
      struct Serializer<std::map<Key, T, Compare, Allocator>>
        : public detail::SequenceSerializer<
              std::map<Key, T, Compare, Allocator>
            , stl_insert::InsertPair
          >
      {};

    template<typename Key, typename T, typename Compare, typename Allocator>
      struct StoreClassVersion<std::map<Key, T, Compare, Allocator>>
      {
        static bool const value = false;
      };

    template<typename Key, typename T, typename Compare, typename Allocator>
      struct Serializer<std::unordered_map<Key, T, Compare, Allocator>>
        : public detail::SequenceSerializer<std::unordered_map<Key, T, Compare, Allocator>>
      {};

    template<typename Key, typename T, typename Compare, typename Allocator>
      struct StoreClassVersion<std::unordered_map<Key, T, Compare, Allocator>>
      {
        static bool const value = false;
      };

}}

#endif


