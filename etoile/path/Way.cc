//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Way.cc
//
// created       julien quintard   [wed apr  7 00:29:12 2010]
// updated       julien quintard   [mon aug  8 14:56:18 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Way.hh>

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
    }

    ///
    /// this constructor creates a way but returns the last element of the path
    /// in _name_.
    ///
    Way::Way(const Way&						way,
	     Slice&						name):
      path(way.path,
	   0,
	   way.path.find_last_of(elle::System::Path::Separator) + 1)
    {
      Length	start;
      Length	end;

      // compute the offsets.
      end = way.path.find_last_of(elle::System::Path::Separator);
      start = way.path.find_first_not_of(elle::System::Path::Separator, end);

      // compute name.
      name.assign(way.path.substr(start, name.npos));
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
      length = (Length)this->path.length();

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
