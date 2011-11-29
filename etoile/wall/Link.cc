//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 16:34:43 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Link.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Link.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Link.hh>
#include <etoile/automaton/Rights.hh>

#include <etoile/journal/Journal.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a new link object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status	Link::Create(
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::Link*	context;
      gear::Actor*	actor;

      enterx(instance(actor),
	     slab(scope, gear::Scope::Annihilate));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Create()\n");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::StatusError)
	escape("unable to supply the scope");

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// allocate an actor.
	actor = new gear::Actor(scope);

	// return the identifier.
	identifier = actor->identifier;

	// apply the create automaton on the context.
	if (automaton::Link::Create(*context) == elle::StatusError)
	  escape("unable to create the link");

	// set the actor's state.
	actor->state = gear::Actor::StateUpdated;

	// waive the actor.
	waive(actor);

	// waive the scope.
	waive(scope);
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method loads a link object given its chemin and initializes
    /// an associated context.
    ///
    elle::Status	Link::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::Link*	context;
      gear::Actor*	actor;

      enterx(slab(scope, gear::Scope::Annihilate),
	     instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
	escape("unable to acquire the scope");

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// allocate an actor.
	actor = new gear::Actor(scope);

	// return the identifier.
	identifier = actor->identifier;

	// locate the object based on the chemin.
	if (chemin.Locate(context->location) == elle::StatusError)
	  escape("unable to locate the link");

	// apply the load automaton on the context.
	if (automaton::Link::Load(*context) == elle::StatusError)
	  escape("unable to load the link");

	// waive the actor.
	waive(actor);

	// waive the scope.
	waive(scope);
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this application locks the given link.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	Link::Lock(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Lock()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method releases a previously locked link.
    ///
    elle::Status	Link::Release(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Release()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method binds a new target way to the given object.
    ///
    elle::Status	Link::Bind(
			  const gear::Identifier&		identifier,
			  const path::Way&			way)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Link*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Bind()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// apply the bind automaton on the context.
	if (automaton::Link::Bind(*context,
				  way) == elle::StatusError)
	  escape("unable to bind the link");

	// set the actor's state.
	actor->state = gear::Actor::StateUpdated;
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method returns the way pointed by this link.
    ///
    elle::Status	Link::Resolve(
			  const gear::Identifier&		identifier,
			  path::Way&				way)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Link*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Resolve()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeRead);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// apply the resolve automaton on the context.
	if (automaton::Link::Resolve(*context,
				     way) == elle::StatusError)
	  escape("unable to resolve the link");
      }
      zone.Unlock();

      leave();
    }

    ///
    /// this method discards the scope, potentially ignoring the
    /// performed modifications.
    ///
    elle::Status	Link::Discard(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Link*	context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// check the permissions before performing the operation in
	// order not to alter the scope should the operation not be
	// allowed.
	if (automaton::Rights::Operate(
	      *context,
	      gear::OperationDiscard) == elle::StatusError)
	  escape("the user does not seem to have the necessary permission for "
		 "discarding this link");

	// specify the closing operation performed by the actor.
	if (actor->Operate(gear::OperationDiscard) == elle::StatusError)
	  escape("this operation cannot be performed by this actor");

	// delete the actor.
	delete actor;

	// waive actor.
	waive(actor);

	// specify the closing operation performed on the scope.
	if (scope->Operate(gear::OperationDiscard) == elle::StatusError)
	  escape("unable to specify the operation being performed "
		 "on the scope");

	// trigger the shutdown.
	if (scope->Shutdown() == elle::StatusError)
	  escape("unable to trigger the shutdown");
      }
      zone.Unlock();

      // depending on the context's state.
      switch (context->state)
	{
	case gear::Context::StateDiscarded:
	case gear::Context::StateStored:
	case gear::Context::StateDestroyed:
	  {
	    //
	    // if the link has been sealed, i.e there is no more actor
	    // operating on it, record it in the journal.
	    //

	    // relinquish the scope: at this point we know there is no
	    // remaining actor.
	    if (gear::Scope::Relinquish(scope) == elle::StatusError)
	      escape("unable to relinquish the scope");

	    // record the scope in the journal.
	    if (journal::Journal::Record(scope) == elle::StatusError)
	      escape("unable to record the scope in the journal");

	    break;
	  }
	default:
	  {
	    //
	    // otherwise, some actors are probably still working on it.
	    //

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status	Link::Store(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Link*	context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// check the permissions before performing the operation in
	// order not to alter the scope should the operation not be
	// allowed.
	if (automaton::Rights::Operate(
	      *context,
	      gear::OperationStore) == elle::StatusError)
	  escape("the user does not seem to have the necessary permission for "
		 "storing this link");

	// specify the closing operation performed by the actor.
	if (actor->Operate(gear::OperationStore) == elle::StatusError)
	  escape("this operation cannot be performed by this actor");

	// delete the actor.
	delete actor;

	// waive actor.
	waive(actor);

	// specify the closing operation performed on the scope.
	if (scope->Operate(gear::OperationStore) == elle::StatusError)
	  escape("unable to specify the operation being performed "
		 "on the scope");

	// trigger the shutdown.
	if (scope->Shutdown() == elle::StatusError)
	  escape("unable to trigger the shutdown");
      }
      zone.Unlock();

      // depending on the context's state.
      switch (context->state)
	{
	case gear::Context::StateDiscarded:
	case gear::Context::StateStored:
	case gear::Context::StateDestroyed:
	  {
	    //
	    // if the link has been sealed, i.e there is no more actor
	    // operating on it, record it in the journal.
	    //

	    // relinquish the scope: at this point we know there is no
	    // remaining actor.
	    if (gear::Scope::Relinquish(scope) == elle::StatusError)
	      escape("unable to relinquish the scope");

	    // record the scope in the journal.
	    if (journal::Journal::Record(scope) == elle::StatusError)
	      escape("unable to record the scope in the journal");

	    break;
	  }
	default:
	  {
	    //
	    // otherwise, some actors are probably still working on it.
	    //

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method destroys a link.
    ///
    elle::Status	Link::Destroy(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::Link*	context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      elle::Hurdle::Zone	zone(scope->hurdle, elle::ModeWrite);

      // protect the access.
      zone.Lock();
      {
	// retrieve the context.
	if (scope->Use(context) == elle::StatusError)
	  escape("unable to retrieve the context");

	// check the permissions before performing the operation in
	// order not to alter the scope should the operation not be
	// allowed.
	if (automaton::Rights::Operate(
	      *context,
	      gear::OperationDestroy) == elle::StatusError)
	  escape("the user does not seem to have the necessary permission for "
		 "destroying this link");

	// specify the closing operation performed by the actor.
	if (actor->Operate(gear::OperationDestroy) == elle::StatusError)
	  escape("this operation cannot be performed by this actor");

	// delete the actor.
	delete actor;

	// waive actor.
	waive(actor);

	// specify the closing operation performed on the scope.
	if (scope->Operate(gear::OperationDestroy) == elle::StatusError)
	  escape("unable to specify the operation being performed "
		 "on the scope");

	// trigger the shutdown.
	if (scope->Shutdown() == elle::StatusError)
	  escape("unable to trigger the shutdown");
      }
      zone.Unlock();

      // depending on the context's state.
      switch (context->state)
	{
	case gear::Context::StateDiscarded:
	case gear::Context::StateStored:
	case gear::Context::StateDestroyed:
	  {
	    //
	    // if the link has been sealed, i.e there is no more actor
	    // operating on it, record it in the journal.
	    //

	    // relinquish the scope: at this point we know there is no
	    // remaining actor.
	    if (gear::Scope::Relinquish(scope) == elle::StatusError)
	      escape("unable to relinquish the scope");

	    // record the scope in the journal.
	    if (journal::Journal::Record(scope) == elle::StatusError)
	      escape("unable to record the scope in the journal");

	    break;
	  }
	default:
	  {
	    //
	    // otherwise, some actors are probably still working on it.
	    //

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method purges a link i.e removes all the blocks of all
    /// the versions associated with this link.
    ///
    elle::Status	Link::Purge(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::Link::Purge()\n");

      // XXX to implement.

      leave();
    }

  }
}
