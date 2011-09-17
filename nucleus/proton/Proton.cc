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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Natural32		Proton::Size::Low;

    ///
    /// XXX
    ///
    elle::Natural32		Proton::Size::Normal;

    ///
    /// XXX
    ///
    elle::Natural32		Proton::Size::High;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the proton module.
    ///
    elle::Status	Proton::Initialize()
    {
      enter();

      //
      // retrieve some descriptor specific configuration values.
      //
      {
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
      }

      leave();
    }

    ///
    /// this method cleans the proton module.
    ///
    elle::Status	Proton::Clean()
    {
      enter();

      // XXX

      leave();
    }

  }
}
