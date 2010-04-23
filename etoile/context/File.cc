//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/File.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [tue apr 20 10:31:58 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/File.hh>

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
    File::File():
      Object::Object(FormatFile),

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
    Status		File::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Context] File" << std::endl;

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
