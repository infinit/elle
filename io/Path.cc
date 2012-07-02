#include <elle/io/Path.hh>
#include <elle/io/Pattern.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a path based on a string.
    ///
    Status              Path::Create(const String&              string)
    {
      // assign the string.
      this->_string = string;

      return Status::Ok;
    }

    ///
    /// this method creates a path according to a given pattern.
    ///
    Status              Path::Create(const Pattern&             pattern)
    {
      // assign the pattern.
      this->_string = pattern.string;

      return Status::Ok;
    }

    ///
    /// this function resolves the given name/value tuple.
    ///
    Status              Path::Complete(const String&            name,
                                       const String&            value)
    {
      String::size_type position;

      // as long as the name is located within the string.
      while ((position = this->_string.find(name)) != String::npos)
        {
          // replace the name with the value.
          this->_string.replace(position, name.length(), value);
        }

      return Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean             Path::operator==(const Path&            element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the internal.
      if (this->_string != element.str())
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Path, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the object's internal.
    ///
    Status              Path::Dump(const Natural32              margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Path] "
                << this->_string
                << std::endl;

      return Status::Ok;
    }

  }
}
