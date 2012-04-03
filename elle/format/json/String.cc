#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "String.hh"

using namespace elle::format::json;

void String::Save(elle::serialize::OutputJSONArchive& ar) const
{
  ar << _value;
}

