//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       elle
//
// file          /home/mycure/infinit/elle/io/Path.cc
//
// created       julien quintard   [mon apr 25 11:12:59 2011]
// updated       julien quintard   [tue apr 26 12:19:59 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Path.hh>

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
    Status		Path::Create(const String&		string)
    {
      enter();

      // assign the string.
      this->string = string;

      leave();
    }

    ///
    /// this method creates a path according to a given pattern.
    ///
    Status		Path::Create(const Pattern&		pattern)
    {
      enter();

      // assign the pattern.
      this->string = pattern.string;

      leave();
    }

    ///
    /// this function resolves the given name/value tuple.
    ///
    Status		Path::Complete(const String&		name,
				       const String&		value)
    {
      size_t		position;

      enter();

      // as long as the name is located within the string.
      while ((position = this->string.find(name)) != String::npos)
	{
	  // replace the name with the value.
	  this->string.replace(position, name.length(), value);
	}

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two objects match.
    ///
    Boolean		Path::operator==(const Path&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal.
      if (this->string != element.string)
	false();

      true();
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
    Status		Path::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Path] "
		<< this->string
		<< std::endl;

      leave();
    }

  }
}
