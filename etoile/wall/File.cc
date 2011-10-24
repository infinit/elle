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

#include <etoile/wall/File.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/File.hh>

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
    /// this method creates a new file object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status	File::Create(
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::File*	context;
      gear::Actor*	actor;

      enterx(instance(actor),
	     slab(scope, gear::Scope::Relinquish));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Create()\n");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::StatusError)
	escape("unable to supply the scope");

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // allocate an actor.
      actor = new gear::Actor(scope);

      // return the identifier.
      identifier = actor->identifier;

      // apply the create automaton on the context.
      if (automaton::File::Create(*context) == elle::StatusError)
	escape("unable to create the file");

      // waive the actor.
      waive(actor);

      // waive the scope.
      waive(scope);

      leave();
    }

    ///
    /// this method loads a file object given its chemin and initializes
    /// an associated context.
    ///
    elle::Status	File::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::File*	context;
      gear::Actor*	actor;

      enterx(instance(actor),
	     slab(scope, gear::Scope::Relinquish));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
	escape("unable to acquire the scope");

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // allocate an actor.
      actor = new gear::Actor(scope);

      // return the identifier.
      identifier = actor->identifier;

      // locate the object based on the chemin.
      if (chemin.Locate(context->location) == elle::StatusError)
	escape("unable to locate the file");

      // apply the load automaton on the context.
      if (automaton::File::Load(*context) == elle::StatusError)
	escape("unable to load the file");

      // waive the actor.
      waive(actor);

      // waive the scope.
      waive(scope);

      leave();
    }

    ///
    /// this application locks the given file.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	File::Lock(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Lock()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method releases a previously locked file.
    ///
    elle::Status	File::Release(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Release()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method writes the file with the given region of data.
    ///
    elle::Status	File::Write(
		          const gear::Identifier&		identifier,
			  const nucleus::Offset&		offset,
			  const elle::Region&			region)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Write()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the write automaton on the context.
      if (automaton::File::Write(*context,
				 offset,
				 region) == elle::StatusError)
	escape("unable to write the file");

      // set the actor's state.
      actor->state = gear::Actor::StateUpdated;

      leave();
    }

    ///
    /// this method reads a region of the file.
    ///
    elle::Status	File::Read(
			  const gear::Identifier&		identifier,
			  const nucleus::Offset&		offset,
			  const nucleus::Size&			size,
			  elle::Region&				region)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Read()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the read automaton on the context.
      if (automaton::File::Read(*context,
				offset,
				size,
				region) == elle::StatusError)
	escape("unable to read the file");

      leave();
    }

    ///
    /// this method adjusts the size of a file.
    ///
    elle::Status	File::Adjust(
			  const gear::Identifier&		identifier,
			  const nucleus::Size&			size)
    {
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Adjust()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the adjust automaton on the context.
      if (automaton::File::Adjust(*context,
				  size) == elle::StatusError)
	escape("unable to adjust the file's size");

      // set the actor's state.
      actor->state = gear::Actor::StateUpdated;

      leave();
    }

    ///
    /// this method discards the scope, potentially ignoring the
    /// performed modifications.
    ///
    elle::Status	File::Discard(
			  const gear::Identifier&		identifier)
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  gear::File&
	  >
	>		callback;
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::File*	context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationDiscard) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      // specify the closing operation performed on the scope.
      if (scope->Operate(gear::OperationDiscard) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the shutdown callback.
      if (scope->Shutdown(callback) == elle::StatusError)
	escape("unable to retrieve the shutdown callback");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // record the scope in the journal.
      if (journal::Journal::Record(scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      leave();
    }

    ///
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status	File::Store(
			  const gear::Identifier&		identifier)
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  gear::File&
	  >
	>		callback;
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::File*	context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationStore) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      // specify the closing operation performed on the scope.
      if (scope->Operate(gear::OperationStore) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the shutdown callback.
      if (scope->Shutdown(callback) == elle::StatusError)
	escape("unable to retrieve the shutdown callback");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // record the scope in the journal.
      if (journal::Journal::Record(scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      leave();
    }

    ///
    /// this method destroys a file.
    ///
    elle::Status	File::Destroy(
			  const gear::Identifier&		identifier)
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  gear::File&
	  >
	>		callback;
      gear::Actor*	actor;
      gear::Scope*	scope;
      gear::File*	context;

      enterx(instance(actor));

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationDestroy) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      // specify the closing operation performed on the scope.
      if (scope->Operate(gear::OperationDestroy) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the shutdown callback.
      if (scope->Shutdown(callback) == elle::StatusError)
	escape("unable to retrieve the shutdown callback");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // record the scope in the journal.
      if (journal::Journal::Record(scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      leave();
    }

    ///
    /// this method purges a file i.e removes all the blocks of all
    /// the versions associated with this file.
    ///
    elle::Status	File::Purge(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
	printf("[etoile] wall::File::Purge()\n");

      // XXX to implement.

      leave();
    }

  }
}
