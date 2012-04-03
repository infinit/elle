#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Bool.hh"

using namespace elle::format::json;

void Bool::Save(elle::serialize::OutputJSONArchive& ar) const
{
  ar.stream() << (_value ? "true" : "false");
}

