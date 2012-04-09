#ifndef ELLE_SERIALIZE_ASSOCIATIVECONTAINER_HH
# define ELLE_SERIALIZE_ASSOCIATIVECONTAINER_HH

# include "SequenceSerializer.hh"

namespace elle { namespace serialize { namespace detail {

    // Base class for stl associative containers
    template<typename Container_>
      struct AssociativeSerializer
        : public SequenceSerializer<Container_> // Save() method is generic enough
      {
        typedef Container_ Container;

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
                  container.insert(el);
                }
            }
      };

}}}

#endif /* ! SEQUENCECONTAINER_HH */




