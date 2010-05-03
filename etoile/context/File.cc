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
// updated       julien quintard   [mon may  3 18:00:42 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/File.hh>
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
    elle::Status	File::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Context] File" << std::endl;

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
