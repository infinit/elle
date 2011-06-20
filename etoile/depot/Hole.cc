//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Hole.cc
//
// created       julien quintard   [thu may 12 14:43:49 2011]
// updated       julien quintard   [tue jun 14 21:55:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Hole.hh>

#include <hole/Hole.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Hole::Origin(nucleus::Address&		address)
    {
      enter();

      // call the Hole.
      if (hole::Hole::Origin(address) == elle::StatusError)
	escape("unable to retrieve the origin");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Hole::Push(const nucleus::Address&	address,
				   const nucleus::Block&	block)
    {
      enter();

      // call the Hole.
      if (hole::Hole::Push(address, block) == elle::StatusError)
	escape("unable to store the block");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Hole::Pull(const nucleus::Address&	address,
				   const nucleus::Version&	version,
				   nucleus::Block&		block)
    {
      enter();

      // call the Hole.
      if (hole::Hole::Pull(address, version, block) == elle::StatusError)
	escape("unable to retrieve the block");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Hole::Wipe(const nucleus::Address&	address)
    {
      enter();

      // call the Hole.
      if (hole::Hole::Wipe(address) == elle::StatusError)
	escape("unable to remove the block");

      leave();
    }

  }
}
