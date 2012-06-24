#include <sstream>
#include <string>

#include <elle/serialize/JSONArchive.hxx>

#include "Dictionary.hh"
#include "Object.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {
      std::string Object::repr() const
      {
        std::ostringstream ss;

        elle::serialize::OutputJSONArchive ar(ss, *this);
        return ss.str();
      }

      Object const& Object::operator [](std::string const& key) const
      {
        return dynamic_cast<Dictionary const&>(*this)[key];
      }

      Object const& Object::operator [](size_t index) const
      {
        return dynamic_cast<Array const&>(*this)[index];
      }

    }
  }
}
