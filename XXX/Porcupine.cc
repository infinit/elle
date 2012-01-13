//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat oct  8 22:42:48 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <XXX/Porcupine.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the porcupine.
    ///
    elle::Status	Porcupine<>::Initialize()
    {
      enter();

      //
      // catalog-specific initialization.
      //
      {
	// initialize the seam.
	if (Seam<nucleus::Catalog>::Initialize() == elle::StatusError)
	  escape("unable to initialize the seam");

	// initialize the quill.
	if (Quill<nucleus::Catalog>::Initialize() == elle::StatusError)
	  escape("unable to initialize the quill");
      }

      // XXX

      leave();
    }

    ///
    /// this method cleans the porcupine.
    ///
    elle::Status	Porcupine<>::Clean()
    {
      enter();

      //
      // catalog-specific cleaning.
      //
      {
	// clean the quill.
	if (Quill<nucleus::Catalog>::Clean() == elle::StatusError)
	  escape("unable to clean the quill");

	// clean the seam.
	if (Seam<nucleus::Catalog>::Clean() == elle::StatusError)
	  escape("unable to clean the seam");
      }

      // XXX

      leave();
    }

  }
}
