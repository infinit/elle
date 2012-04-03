#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Int.hh"

using namespace elle::format::json;

void Int::Save(elle::serialize::OutputJSONArchive& ar) const
{
  ar << _value;
}

