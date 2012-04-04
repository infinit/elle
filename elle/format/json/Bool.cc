#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Bool.hh"

using namespace elle::format::json;

namespace elle { namespace format { namespace json { namespace detail {

    template<>
    void Bool::Save(elle::serialize::OutputJSONArchive& ar) const
    {
      ar.stream() << (_value ? "true" : "false");
    }

}}}}

