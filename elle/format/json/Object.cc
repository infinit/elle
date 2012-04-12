#include <sstream>
#include <string>

#include "elle/serialize/JSONArchive.hxx"

#include "Object.hh"

using namespace elle::format::json;

std::string Object::repr() const
{
  std::ostringstream ss;

  elle::serialize::OutputJSONArchive ar(ss, *this);
  return ss.str();
}
