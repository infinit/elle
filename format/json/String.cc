#include <ostream>

#include "elle/serialize/JSONArchive.hxx"

#include "String.hh"

using namespace elle::format::json;

namespace elle { namespace format { namespace json { namespace detail {

    template<>
    void String::Save(elle::serialize::OutputJSONArchive& ar) const
    {
      ar << _value;
    }

}}}}
