//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 31 14:28:20 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/local/Machine.hh>
#include <hole/Hole.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

//
// ---------- holeable --------------------------------------------------------
//

      ///
      /// this method stores an immutable block.
      ///
      elle::Status	Machine::Put(const nucleus::Address&	address,
				     const nucleus::ImmutableBlock& block)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::local::Put[Immutable]()\n");

	// does the block already exist.
	if (block.Exist(Hole::Implementation->network,
			address) == elle::StatusTrue)
	  escape("this immutable block seems to already exist");

	// store the block.
	if (block.Store(Hole::Implementation->network,
			address) == elle::StatusError)
	  escape("unable to store the block");

	leave();
      }

      ///
      /// this method stores a mutable block.
      ///
      elle::Status	Machine::Put(const nucleus::Address&	address,
				     const nucleus::MutableBlock& block)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::local::Put[Mutable]()\n");

	// validate the block, depending on its component.
	//
	// indeed, the Object component requires as additional block for
	// being validated.
	switch (address.component)
	  {
	  case nucleus::ComponentObject:
	    {
	      const nucleus::Object*	object =
		static_cast<const nucleus::Object*>(&block);

	      // validate the object according to the presence of
	      // a referenced access block.
	      if (object->meta.access != nucleus::Address::Null)
		{
		  nucleus::Access	access;

		  // load the access block.
		  if (Hole::Pull(object->meta.access,
				 nucleus::Version::Last,
				 access) == elle::StatusError)
		    escape("unable to load the access block");

		  // validate the object, providing the
		  if (object->Validate(address, access) == elle::StatusError)
		    escape("unable to validate the object");
		}
	      else
		{
		  // validate the object.
		  if (object->Validate(
			address,
			nucleus::Access::Null) == elle::StatusError)
		    escape("unable to validate the object");
		}

	      break;
	    }
	  default:
	    {
	      // validate the block through the common interface.
	      if (block.Validate(address) == elle::StatusError)
		escape("the block seems to be invalid");

	      break;
	    }
	  case nucleus::ComponentUnknown:
	    {
	      escape("unknown component '%u'",
		     address.component);
	    }
	  }

	// does the block already exist.
	if (block.Exist(Hole::Implementation->network,
			address,
			nucleus::Version::Last) == elle::StatusTrue)
	  {
	    nucleus::MutableBlock*	current;

	    enterx(instance(current));

	    // build a block according to the component.
	    if (nucleus::Nucleus::Factory.Build(address.component,
						current) == elle::StatusError)
	      escape("unable to build the block");

	    // load the latest version.
	    if (current->Load(Hole::Implementation->network,
			      address,
			      nucleus::Version::Last) == elle::StatusError)
	      escape("unable to load the current version");

	    // does the given block derive the current version.
	    if (!(block.version > current->version))
	      escape("the block to store does not seem to derive the current "
		     "version");

	    // delete the current instance.
	    delete current;

	    // waive.
	    waive(current);

	    // release.
	    release();
	  }

	// store the block.
	if (block.Store(Hole::Implementation->network,
			address) == elle::StatusError)
	  escape("unable to store the block");

	leave();
      }

      ///
      /// this method retrieves an immutable block.
      ///
      elle::Status	Machine::Get(const nucleus::Address&	address,
				     nucleus::ImmutableBlock&	block)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::local::Get[Immutable]()\n");

	// does the block exist.
	if (block.Exist(Hole::Implementation->network,
			address) == elle::StatusFalse)
	  escape("the block does not seem to exist");

	// load the block.
	if (block.Load(Hole::Implementation->network,
		       address) == elle::StatusError)
	  escape("unable to load the block");

	// validate the block.
	if (block.Validate(address) == elle::StatusError)
	  escape("the block seems to be invalid");

	leave();
      }

      ///
      /// this method retrieves a mutable block.
      ///
      elle::Status	Machine::Get(const nucleus::Address&	address,
				     const nucleus::Version&	version,
				     nucleus::MutableBlock&	block)
      {
	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::local::Get[Mutable]()\n");

	// does the block exist.
	if (block.Exist(Hole::Implementation->network,
			address, version) == elle::StatusFalse)
	  escape("the block does not seem to exist");

	// load the block.
	if (block.Load(Hole::Implementation->network,
		       address, version) == elle::StatusError)
	  escape("unable to load the block");

	// validate the block, depending on its component.
	//
	// indeed, the Object component requires as additional block for
	// being validated.
	switch (address.component)
	  {
	  case nucleus::ComponentObject:
	    {
	      const nucleus::Object*	object =
		static_cast<const nucleus::Object*>(&block);

	      // validate the object according to the presence of
	      // a referenced access block.
	      if (object->meta.access != nucleus::Address::Null)
		{
		  nucleus::Access	access;

		  // load the access block.
		  if (Hole::Pull(object->meta.access,
				 nucleus::Version::Last,
				 access) == elle::StatusError)
		    escape("unable to load the access block");

		  // validate the object, providing the
		  if (object->Validate(address, access) == elle::StatusError)
		    escape("unable to validate the object");
		}
	      else
		{
		  // validate the object.
		  if (object->Validate(
			address,
			nucleus::Access::Null) == elle::StatusError)
		    escape("unable to validate the object");
		}

	      break;
	    }
	  default:
	    {
	      // validate the block through the common interface.
	      if (block.Validate(address) == elle::StatusError)
		escape("the block seems to be invalid");

	      break;
	    }
	  case nucleus::ComponentUnknown:
	    {
	      escape("unknown component '%u'",
		     address.component);
	    }
	  }

	leave();
      }

      ///
      /// this method removes a block.
      ///
      elle::Status	Machine::Kill(const nucleus::Address&	address)
      {
	nucleus::Block	block;

	enter();

	// debug.
	if (Infinit::Configuration.hole.debug == true)
	  printf("[hole] implementations::local::Kill()\n");

	// treat the request depending on the nature of the block which
	// the addres indicates.
	switch (address.family)
	  {
	  case nucleus::FamilyContentHashBlock:
	    {
	      nucleus::ImmutableBlock	ib;

	      // erase the immutable block.
	      if (ib.Erase(Hole::Implementation->network,
			   address) == elle::StatusError)
		escape("unable to erase the block");

	      break;
	    }
	  case nucleus::FamilyPublicKeyBlock:
	  case nucleus::FamilyOwnerKeyBlock:
	  case nucleus::FamilyImprintBlock:
	    {
	      nucleus::MutableBlock	mb;

	      // retrieve the mutable block.
	      if (mb.Erase(Hole::Implementation->network,
			   address) == elle::StatusError)
		escape("unable to erase the block");

	      break;
	    }
	  default:
	    {
	      escape("unknown block family");
	    }
	  }

	leave();
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this method dumps the implementation.
      ///
      elle::Status	Machine::Dump(const elle::Natural32	margin) const
      {
	elle::String	alignment(margin, ' ');

	enter();

	std::cout << alignment << "[Machine]" << std::endl;

	leave();
      }

    }
  }
}
