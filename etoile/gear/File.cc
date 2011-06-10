//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/File.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [fri jun  3 11:47:16 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/File.hh>
#include <etoile/gear/Nature.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    File::File():
      Object(NatureFile),

      contents(NULL)
    {
    }

    ///
    /// the destructor
    ///
    File::~File()
    {
      // release the contents.
      if (this->contents != NULL)
	delete this->contents;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents along the the inherited object context.
    ///
    elle::Status	File::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[File]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the inherited object");

      // dump the contents.
      if (this->contents != NULL)
	{
	  if (this->contents->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the contents");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Contents] " << none << std::endl;
	}

      leave();
    }

  }
}
