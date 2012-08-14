#include <ostream>

#include "Null.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      Null::Null()
      {}

      Null::~Null()
      {}

      void
      Null::repr(std::ostream& out) const
      {
        out << "null";
      }

      std::unique_ptr<Object>
      Null::clone() const
      {
        return std::unique_ptr<Object>(new Null);
      }

      bool
      Null::operator ==(Object const& other) const
      {
        return other == *this;
      }

      bool
      Null::operator ==(Null const&) const
      {
        return true;
      }

      Null const null;

    }
  }
}
