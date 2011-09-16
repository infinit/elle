//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu sep 15 18:33:06 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Porcupine.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Porcupine::Create(const elle::Natural32	size)
    {
      enter();

      // XXX
      //this->bpt = new t_bpt(porcupine);
      struct s_bpt_porcupine x;

      leave();
    }

  }
}
