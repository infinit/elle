#include <elle/io/Pattern.hh>

#include <iostream>

namespace elle
{
  namespace io
  {

//
// ---------- constructors & destructors --------------------------------------
//

    Pattern::Pattern()
    {
    }

    Pattern::Pattern(elle::String const& string):
      string(string)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a pattern.
    ///
    Status Pattern::Create(const String&           string)
    {
      // assign the string.
      this->string = string;

      return Status::Ok;
    }

    ///
    /// this method check if two objects match.
    ///
    bool Pattern::operator==(const Pattern&      element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->string == element.string);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object's internal.
    ///
    Status              Pattern::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Pattern] "
                << this->string << std::endl;

      return Status::Ok;
    }

  }
}
