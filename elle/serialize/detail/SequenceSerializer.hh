#ifndef ELLE_SERIALIZE_SEQUENCECONTAINER_HH
# define ELLE_SERIALIZE_SEQUENCECONTAINER_HH

# include <boost/foreach.hpp>

# include <elle/serialize/ArchiveSerializer.hxx>

namespace elle { namespace serialize { namespace detail {

    // Base class for stl sequences
    template<typename Container_>
      struct SequenceSerializer
        : public SplittedSerializer<Container_>
      {
        typedef Container_ Container;

        template<typename Archive>
          static inline Save(Archive& ar, Container const& container, unsigned int)
            {
              static_assert(
                  StoreClassVersion<Container>::value == false,
                  "oups, should not store class version"
              );
              typedef typename Archive::SequenceSizeType SizeType;
              static_assert(
                  std::is_unsigned<SizeType>::value,
                  "SequenceSizeType has to be unsigned"
              );
              size_t size = container.size();
              if (size > static_cast<size_t>(static_cast<SizeType>(-1)))
                throw std::runtime_error("Sequence too big");
              ar << static_cast<SizeType>(size);
              BOOST_FOREACH(auto& el, container)
                {
                  ar << el;
                }
            }

        template<typename Archive>
          static inline Load(Archive& ar, Container& container, unsigned int)
            {
              static_assert(
                  StoreClassVersion<Container>::value == false,
                  "oups, should not store class version"
              );
              typedef typename Archive::SequenceSizeType SizeType;
              static_assert(
                  std::is_unsigned<SizeType>::value,
                  "SequenceSizeType has to be unsigned"
              );
              SizeType size;
              ar >> size;
              for (SizeType i = 0; i < size; ++i)
                {
                  /// XXX copy involved
                  /// XXX value_type ctor not customizable
                  typename Container::value_type el;
                  ar >> el;
                  container.push_back(el);
                }
            }
      };

}}}

#endif /* ! SEQUENCECONTAINER_HH */



