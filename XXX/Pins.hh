//
// ---------- header ----------------------------------------------------------
//
// project       XXX
//
// license       infinit
//
// author        julien quintard   [sun mar 18 16:01:45 2012]
//

#ifndef NUCLEUS_PROTON_PINS_HH
#define NUCLEUS_PROTON_PINS_HH

namespace nucleus
{
  namespace proton
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// XXX use for checking the porcupine.
    ///
    typedef elle::Natural8      Pins;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// XXX
    ///
    const Pins                  PinNone = 0;
    const Pins                  PinParent = (1 << 1);
    const Pins                  PinNeighbours = (1 << 2);
    const Pins                  PinKey = (1 << 3);
    const Pins                  PinAddress = (1 << 4);
    const Pins                  PinAll =
      PinParent | PinNeighbours | PinKey | PinAddress;

  }
}

#endif
