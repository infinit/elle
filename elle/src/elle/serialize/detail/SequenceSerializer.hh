#ifndef ELLE_SERIALIZE_SEQUENCECONTAINER_HH
# define ELLE_SERIALIZE_SEQUENCECONTAINER_HH

# include <elle/serialize/Serializer.hh>

namespace elle
{
  namespace serialize
  {
    namespace detail
    {
      namespace stl_insert
      {
        // XXX do emplace back when possible
        template <typename Container>
        struct PushBack
        {
          typedef typename Container::value_type value_type;

          template <typename Archive>
          static inline
          void
          insert_elem(Archive& ar,
                      Container& container)
          {
            container.push_back(ar.template construct_value<value_type>());
          }
        };

        // XXX do emplace back when possible
        template <typename Container>
        struct Insert
        {
          typedef typename Container::value_type value_type;

          template <typename Archive>
          static inline
          void
          insert_elem(Archive& ar,
                      Container& container)
          {
            container.insert(ar.template construct_value<value_type>());
          }
        };
      } // !ns stl_insert

      // Base class for stl sequences
      template <typename Container_,
                template <typename> class InsertMethod = stl_insert::PushBack>
      struct SequenceSerializer:
        public elle::serialize::SplitSerializer<Container_>
      {
        typedef Container_ Container;

        template <typename Archive>
        static inline
        void Save(Archive& ar,
                  Container const& container,
                  unsigned int /* version */)
        {
          static_assert(StoreFormat<Container>::value == false,
                        "oups, should not store class version");
          typedef typename Archive::SequenceSizeType size_type;
          static_assert(std::is_unsigned<size_type>::value,
                        "SequenceSizeType has to be unsigned");
          size_t size = container.size();
          if (size > static_cast<size_t>(static_cast<size_type>(-1)))
            throw std::runtime_error("Sequence too big");
          ar << static_cast<size_type>(size);
          for (auto const& el: container)
            ar << el;
        }

        template <typename Archive>
        static inline
        void
        Load(Archive& ar,
             Container& container,
             unsigned int /* version */)
        {
          static_assert(StoreFormat<Container>::value == false,
                        "oups, should not store class version");
          typedef typename Archive::SequenceSizeType size_type;
          static_assert(std::is_unsigned<size_type>::value,
                        "SequenceSizeType has to be unsigned");
          size_type size;
          ar >> size;
          for (size_type i = 0; i < size; ++i)
            InsertMethod<Container>::insert_elem(ar, container);
        }
      };
    }
  }
}

#endif /* ! SEQUENCECONTAINER_HH */
