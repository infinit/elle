//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Link.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [tue apr 20 10:32:06 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Link.hh>

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
    Link::Link():
      Object::Object(FormatLink),

      contents(NULL)
    {
    }

    ///
    /// the destructor
    ///
    Link::~Link()
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
    Status		Link::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Context] Link" << std::endl;

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
