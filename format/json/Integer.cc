#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Integer.hh"

namespace elle { namespace format { namespace json { namespace detail {


    template<>
    void Integer::Save(elle::serialize::OutputJSONArchive& ar) const
    {
      ar << _value;
    }

}}}}

