//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed apr  7 00:29:12 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Way.hh>
#include <elle/utility/Unicode.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null way.
    ///
    const Way			Way::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor
    ///
    Way::Way()
    {
    }

    ///
    /// copy constructor
    ///
    Way::Way(const Way&						way):
      Object(way),

      path(way.path)
    {
    }

    ///
    /// character constructor
    ///
    Way::Way(const elle::Character&				character):
      path(1, character)
    {
    }

    ///
    /// string constructor
    ///
    Way::Way(const elle::String&				string):
      path(string)
    {
      if (!path.empty() && path[0] != elle::System::Path::Separator)
        path.clear();
    }


    ///
    /// wide char constructor
    ///
    Way::Way(const wchar_t *                                    u16_str):
      path()
    {
      char *    str      = NULL;
      size_t    str_size = 0;

      if (elle::utility::Utf16To8(u16_str,
				  -1,
				  &str,
				  &str_size) == elle::StatusError)
	fail("failed to convert the path to uft8");

      if (str_size > 0 && str[0] == elle::System::Path::Separator)
        path.assign(str, str_size);

      free(str);
    }


    ///
    /// this constructor creates a way but returns the last element of the path
    /// in _name_.
    ///
    Way::Way(const Way&						way,
	     Slice&						name):
      path()
    {
      name.clear();

      elle::String::size_type	last_slash;

      // find the position.
      last_slash = way.path.rfind(elle::System::Path::Separator);

      // check if way.path contains at least a slash
      if (last_slash == elle::String::npos)
        return;

      // extract path and name
      path.assign(way.path.substr(0, last_slash == 0 ? 1 : last_slash));
      name.assign(way.path.substr(last_slash + 1, way.path.npos));
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the length of the way.
    ///
    elle::Status	Way::Capacity(Length&			length) const
    {
      enter();

      // return the length.
      length = static_cast<Length>(this->path.length());

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Way::operator==(const Way&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes..
      if (this->path != element.path)
	false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Way, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the object.
    ///
    elle::Status	Way::Dump(elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Way] " << this->path << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    elle::Status	Way::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the target.
      if (archive.Serialize(this->path) == elle::StatusError)
	escape("unable to serialize the path");

      leave();
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status	Way::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the target.
      if (archive.Extract(this->path) == elle::StatusError)
	escape("unable to extract the path");

      leave();
    }

  }
}
