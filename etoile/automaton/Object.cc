//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 12:47:31 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Access.hh>

#include <etoile/depot/Depot.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method fetches the object and opens the access block, should
    /// this block be present.
    ///
    elle::Status	Object::Load(
			  gear::Object&				context,
			  const nucleus::Location&		location)
    {
      enter();

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
	leave();

      // set the location.
      context.location = location;

      // retrieve the object block.
      if (depot::Depot::Pull(context.location.address,
			     context.location.version,
			     context.object) == elle::StatusError)
	escape("unable to retrieve the object block");

      // compute the base in order to seal the block's original state.
      if (context.object._base.Create(context.object) == elle::StatusError)
	escape("unable to compute the base");

      // set the context's state.
      context.state = gear::Context::StateInitialized;

      leave();
    }

    ///
    /// this method generates an Information object which summarises the
    /// object's meta data.
    ///
    elle::Status	Object::Information(
			  gear::Object&				context,
			  miscellaneous::Information&		information)
    {
      enter();

      // generate the information based on the object.
      if (information.Create(context.object) == elle::StatusError)
	escape("unable to generate the information");

      leave();
    }

    ///
    /// this method permanently destroys the object along with all its
    /// history i.e all its versions.
    ///
    /// therefore, all the blocks are marked as needing to be removed.
    ///
    /// note however that although the past versions of the objects will
    /// be destroyed along the way, the referenced access and data blocks
    /// will not.
    ///
    /// although the destroy functionality may suffice for most cases i.e
    /// for objects with a short history or with a small amount of data,
    /// in other cases, the amount of storage capacity lost---until the
    /// blocks expire---may be too important.
    ///
    /// therefore, the user willing to completely remove the blocks
    /// associated with an object, no matter the version, should take a
    /// look at the Purge() functionality.
    ///
    elle::Status	Object::Destroy(
			  gear::Object&				context)
    {
      enter();

      // open the access.
      if (Access::Open(context) == elle::StatusError)
	escape("unable to open the access block");

      // destroy the access.
      if (Access::Destroy(context) == elle::StatusError)
	escape("unable to destroy the access");

      // mark the object as needing to be removed.
      if (context.transcript.Wipe(context.location.address) ==
	  elle::StatusError)
	escape("unable to record the object for removal");

      // set the context's state.
      context.state = gear::Context::StateDestroyed;

      leave();
    }

    ///
    /// this method terminates the automaton by making the whole object
    /// consistent according to the set of modifications having been performed.
    ///
    elle::Status	Object::Store(
			  gear::Object&				context)
    {
      enter();

      // close the access.
      if (Access::Close(context) == elle::StatusError)
	escape("unable to close the access");

      // if the object has been modified i.e is dirty.
      if (context.object._state == nucleus::StateDirty)
	{
	  // seal the object.
	  if (context.object.Seal(agent::Agent::Identity.pair.k,
				  context.access) == elle::StatusError)
	    escape("unable to seal the object");

	  // mark the block as needing to be stored.
	  if (context.transcript.Push(context.location.address,
				      &context.object) == elle::StatusError)
	    escape("unable to record the object for storing");
	}

      // set the context's state.
      context.state = gear::Context::StateStored;

      leave();
    }

  }
}
