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
// updated       julien quintard   [thu jun 23 13:44:50 2011]
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
    /// this method joins the network.
    ///
    elle::Status	Local::Join()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method leaves the network.
    ///
    elle::Status	Local::Leave()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method stores an immutable block.
    ///
    elle::Status	Local::Put(const nucleus::Address&	address,
				   const nucleus::ImmutableBlock& block)
    {
      enter();

      // does the block already exist.
      if (block.Exist(this->network, address) == elle::StatusTrue)
	escape("this immutable block seems to already exist");

      // store the block.
      if (block.Store(this->network, address) == elle::StatusError)
	escape("unable to store the block");

      leave();
    }

    ///
    /// this method stores a mutable block.
    ///
    elle::Status	Local::Put(const nucleus::Address&	address,
				   const nucleus::MutableBlock&	block)
    {
      enter();

      // does the block already exist.
      if (block.Exist(this->network,
		      address,
		      nucleus::Version::Last) == elle::StatusTrue)
	{
	  nucleus::MutableBlock*	current;

	  // build a block according to the component.
	  if (nucleus::Nucleus::Factory.Build(address.component,
					      current) == elle::StatusError)
	    escape("unable to build the block");

	  // load the latest version.
	  if (current->Load(this->network,
			    address,
			    nucleus::Version::Last) == elle::StatusError)
	    escape("unable to load the current version");

	  // does the given block derive the current version.
	  if (*current >= block)
	    escape("the block to store does not seem to derive the current "
		   "version");

	  // delete the current instance.
	  delete current;
	}

      // store the block.
      if (block.Store(this->network, address) == elle::StatusError)
	escape("unable to store the block");

      leave();
    }

    ///
    /// this method retrieves an immutable block.
    ///
    elle::Status	Local::Get(const nucleus::Address&	address,
				   nucleus::ImmutableBlock&	block)
    {
      enter();

      // does the block exist.
      if (block.Exist(this->network, address) == elle::StatusFalse)
	escape("the block does not seem to exist");

      // load the block.
      if (block.Load(this->network, address) == elle::StatusError)
	escape("unable to load the block");

      // validate the block.
      if (block.Validate(address) == elle::StatusFalse)
	escape("the block seems to be invalid");

      leave();
    }

    ///
    /// this method retrieves a mutable block.
    ///
    elle::Status	Local::Get(const nucleus::Address&	address,
				   const nucleus::Version&	version,
				   nucleus::MutableBlock&	block)
    {
      enter();

      // does the block exist.
      if (block.Exist(this->network, address, version) == elle::StatusFalse)
	escape("the block does not seem to exist");

      // load the block.
      if (block.Load(this->network, address, version) == elle::StatusError)
	escape("unable to load the block");

      // validate the block.
      if (block.Validate(address) == elle::StatusFalse)
	escape("the block seems to be invalid");

      leave();
    }

    ///
    /// this method removes a block.
    ///
    elle::Status	Local::Kill(const nucleus::Address&	address)
    {
      nucleus::Block	block;

      enter();

      // XXX to remove warnings
      nucleus::Address a = address;

      // erase the block.
      // XXX history -> no deletion
      //if (block.Erase(network, address) == elle::StatusError)
      //escape("unable to erase the block");

      leave();
    }

  }
}
