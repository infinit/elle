#ifndef ELLE_SERIALIZE_MAPSERIALIZER_HXX
# define ELLE_SERIALIZE_MAPSERIALIZER_HXX

# include <unordered_map>
# include <map>

# include "detail/SequenceSerializer.hh"

# include "PairSerializer.hxx"

namespace elle
{
  namespace serialize
  {
    namespace stl_insert
    {
      template <typename Container>
      struct InsertPair
      {
        typedef typename Container::value_type::first_type first_type;
        typedef typename Container::value_type::second_type second_type;
        typedef std::pair<
          typename std::remove_cv<first_type>::type,
          typename std::remove_cv<second_type>::type
        > value_type;

        template <typename Archive>
        static inline
        void insert_elem(Archive& ar,
                         Container& container)
        {
          container.insert(ar.template construct_value<value_type>());
        }
      };
    } // !ns stl_insert

    template <typename Key,
              typename T,
              typename Compare,
              typename Allocator>
    struct Serializer<std::map<Key, T, Compare, Allocator>>:
      public detail::SequenceSerializer<std::map<Key, T, Compare, Allocator>,
                                        stl_insert::InsertPair>
    {};

    template <typename Key,
              typename T,
              typename Compare,
              typename Allocator>
    struct StoreFormat<std::map<Key, T, Compare, Allocator>>
    {
      static bool const value = false;
    };

    template <typename Key,
              typename T,
              typename Compare,
              typename Allocator>
    struct Serializer<std::unordered_map<Key, T, Compare, Allocator>>:
      public detail::SequenceSerializer<
        std::unordered_map<Key, T, Compare, Allocator>,
        stl_insert::InsertPair>
    {};

    template <typename Key,
              typename T,
              typename Compare,
              typename Allocator>
    struct StoreFormat<std::unordered_map<Key, T, Compare, Allocator>>
    {
      static bool const value = false;
    };
  }
}

#endif
