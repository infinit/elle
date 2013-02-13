#ifndef ELLE_SERIALIZE_ASSOCIATIVECONTAINER_HH
# define ELLE_SERIALIZE_ASSOCIATIVECONTAINER_HH

# include "SequenceSerializer.hh"

namespace elle { namespace serialize { namespace detail {

    // Base class for stl associative containers
    template<typename Container_>
      struct AssociativeSerializer
        : public SequenceSerializer<Container_, stl_insert::Insert>
      {};

}}}

#endif /* ! SEQUENCECONTAINER_HH */




