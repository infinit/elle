//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author           [tue sep 20 12:15:09 2011]
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
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Porcupine<>::Initialize()
    {
      enter();

      /*
	if (hole::Hole::Descriptor.Get(
	      "general", "size.low",
	      Proton::Size::Low) == elle::StatusError)
	  escape("unable to retrieve the descriptor's value");

	if (hole::Hole::Descriptor.Get(
	      "general", "size.normal",
	      Proton::Size::Normal) == elle::StatusError)
	  escape("unable to retrieve the descriptor's value");

	if (hole::Hole::Descriptor.Get(
	      "general", "size.High",
	      Proton::Size::High) == elle::StatusError)
	  escape("unable to retrieve the descriptor's value");
      */

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Porcupine<>::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

  }
}
