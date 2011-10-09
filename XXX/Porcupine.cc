//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien.quintard   [sat oct  8 22:42:48 2011]
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
    /// XXX
    ///
    elle::Status	Porcupine<>::Initialize()
    {
      enter();

      // initialize the seam.
      if (Seam<nucleus::Catalog>::Initialize() == elle::StatusError)
	escape("unable to initialize the seam");

      // initialize the quill.
      if (Quill<nucleus::Catalog>::Initialize() == elle::StatusError)
	escape("unable to initialize the quill");

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Porcupine<>::Clean()
    {
      enter();

      // clean the quill.
      if (Quill<nucleus::Catalog>::Clean() == elle::StatusError)
	escape("unable to clean the quill");

      // clean the seam.
      if (Seam<nucleus::Catalog>::Clean() == elle::StatusError)
	escape("unable to clean the seam");

      // XXX

      leave();
    }

  }
}
