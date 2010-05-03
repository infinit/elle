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
// updated       julien quintard   [mon may  3 12:52:45 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Link.hh>
#include <etoile/context/Format.hh>

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
    elle::Status	Link::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Context] Link" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the inherited object");

      // dump the contents.
      if (this->contents != NULL)
	if (this->contents->Dump(margin + 4) == elle::StatusError)
	  escape("unable to dump the contents");

      leave();
    }

  }
}
