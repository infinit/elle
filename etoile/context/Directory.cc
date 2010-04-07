//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Directory.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [tue apr  6 23:09:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Directory.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Directory::Directory():
      Object::Object(FormatDirectory),

      contents(NULL)
    {
    }

    ///
    /// the destructor
    ///
    Directory::~Directory()
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
    Status		Directory::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Directory]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == StatusError)
	escape("unable to dump the inherited object");

      // dump the contents.
      if (this->contents != NULL)
	if (this->contents->Dump(margin + 4) == StatusError)
	  escape("unable to dump the contents");

      leave();
    }

  }
}
