#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Null.hh"

using namespace elle::format::json;

Null const elle::format::json::null{};

void Null::Save(elle::serialize::OutputJSONArchive& ar) const
{
  ar.stream() << "null";
}


