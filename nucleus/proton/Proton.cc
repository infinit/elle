//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 14:00:34 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Proton.hh>

#include <hole/Hole.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the proton module.
    ///
    elle::Status	Proton::Initialize()
    {
      enter();

      // initialize the porcupine data structure.
      if (Porcupine<>::Initialize() == elle::StatusError)
	escape("unable to initialize the porcupine");

      leave();
    }

    ///
    /// this method cleans the proton module.
    ///
    elle::Status	Proton::Clean()
    {
      enter();
 
      // clean the porcupine data structure.
      if (Porcupine<>::Clean() == elle::StatusError)
	escape("unable to clean the porcupine");

      leave();
    }

  }
}
