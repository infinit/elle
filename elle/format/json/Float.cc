#include <ostream>

#include "elle/serialize/JSONArchive.hh"

#include "Float.hh"

using namespace elle::format::json;

void Float::Save(elle::serialize::OutputJSONArchive& ar) const
{
  ar << _value;
}

