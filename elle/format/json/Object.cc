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

      std::string
      Object::repr() const
      {
        std::ostringstream ss;

        elle::serialize::OutputJSONArchive ar(ss, *this);
        return ss.str();
      }

      Object const&
      Object::operator [](std::string const& key) const
      {
        return dynamic_cast<Dictionary const&>(*this)[key];
      }

      Object const&
      Object::operator [](size_t index) const
      {
        return dynamic_cast<Array const&>(*this)[index];
      }

      Array&
      Object::as_array()
      {
        return dynamic_cast<Array&>(*this);
      }

      Bool&
      Object::as_bool()
      {
        return dynamic_cast<Bool&>(*this);
      }

      Dictionary&
      Object::as_dictionary()
      {
        return dynamic_cast<Dictionary&>(*this);
      }

      Float&
      Object::as_float()
      {
        return dynamic_cast<Float&>(*this);
      }

      Integer&
      Object::as_integer()
      {
        return dynamic_cast<Integer&>(*this);
      }

      Null&
      Object::as_null()
      {
        return dynamic_cast<Null&>(*this);
      }

      String&
      Object::as_string()
      {
        return dynamic_cast<String&>(*this);
      }


      Array const&
      Object::as_array() const
      {
        return dynamic_cast<Array const&>(*this);
      }

      Bool const&
      Object::as_bool() const
      {
        return dynamic_cast<Bool const&>(*this);
      }

      Dictionary const&
      Object::as_dictionary() const
      {
        return dynamic_cast<Dictionary const&>(*this);
      }

      Float const&
      Object::as_float() const
      {
        return dynamic_cast<Float const&>(*this);
      }

      Integer const&
      Object::as_integer() const
      {
        return dynamic_cast<Integer const&>(*this);
      }

      Null const&
      Object::as_null() const
      {
        return dynamic_cast<Null const&>(*this);
      }

      String const&
      Object::as_string() const
      {
        return dynamic_cast<String const&>(*this);
      }

    }
  }
}
