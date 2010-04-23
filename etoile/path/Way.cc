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
// updated       julien quintard   [thu apr 22 12:29:26 2010]
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
      path(way.path)
    {
    }

    ///
    /// simple constructor
    ///
    Way::Way(const String&					path):
      path(path)
    {
    }

    ///
    /// this constructor creates a way but returns the last element of the path
    /// in _name_.
    ///
    Way::Way(const Way&						way,
	     Slice&						name):
      path(way.path, 0, way.path.find_last_of(System::Path::Separator))
    {
      Length	start;
      Length	end;

      // compute the offsets.
      end = way.path.find_last_of(System::Path::Separator);
      start = way.path.find_first_not_of(System::Path::Separator, end);

      // compute name.
      name.assign(way.path.substr(start, name.npos));
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the length of the way.
    ///
    Status		Way::Capacity(Length&			length) const
    {
      enter();

      // return the length.
      length = (Length)this->path.length();

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Way);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the object.
    ///
    Status		Way::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');

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
    Status		Way::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the target.
      if (archive.Serialize(this->path) == StatusError)
	escape("unable to serialize the path");

      leave();
    }

    ///
    /// this method extracts the object.
    ///
    Status		Way::Extract(Archive&		archive)
    {
      enter();

      // extract the target.
      if (archive.Extract(this->path) == StatusError)
	escape("unable to extract the path");

      leave();
    }

  }
}
