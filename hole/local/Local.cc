//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/local/Local.cc
//
// created       julien quintard   [thu may 12 10:27:04 2011]
// updated       julien quintard   [sat may 14 12:55:34 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/local/Local.hh>

namespace hole
{
  namespace local
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Local::Local(const nucleus::Network&			network):
      Holeable(network)
    {
    }

//
// ---------- holeable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Local::Join()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Local::Leave()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Local::Put(const nucleus::Address&	address,
				   const nucleus::Block&	block)
    {
      enter();

      // store the block.
      if (block.Store(this->network,
		      address) == elle::StatusError)
	escape("unable to store the block");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Local::Get(const nucleus::Address&	address,
				   nucleus::Block&		block)
    {
      enter();

      // does the block exist.
      if (block.Exist(this->network,
		      address) == elle::StatusFalse)
	escape("the block does not seem to exist");

      // load the block.
      if (block.Load(this->network,
		     address) == elle::StatusError)
	escape("unable to load the block");

      // validate the block.
      if (block.Validate(address) == elle::StatusFalse)
	escape("the block seems to be invalid");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Local::Gather(const nucleus::Address&	address,
				      nucleus::Block&		block)
    {
      // forward the call to Get().
      return (this->Get(address, block));
    }

    ///
    /// XXX
    ///
    elle::Status	Local::Erase(const nucleus::Address&	address)
    {
      nucleus::Block	block;

      enter();

      // erase the block.
      if (block.Erase(network, address) == elle::StatusError)
	escape("unable to erase the block");

      leave();
    }

  }
}
