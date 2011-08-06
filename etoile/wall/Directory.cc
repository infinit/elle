//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Directory.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [fri aug  5 12:16:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Directory.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/Gear.hh>
#include <etoile/gear/Operation.hh>

#include <etoile/automaton/Directory.hh>

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
    /// this method creates a new directory object.
    ///
    /// note however that the object is not attached to the hierarchy
    /// and is therefore considered as orphan.
    ///
    elle::Status	Directory::Create(
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::Directory*	context;
      gear::Actor*	actor;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Create()\n");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::StatusError)
	escape("unable to supply the scope");

      // retrieve the context.
      if (scope->Use<gear::NatureDirectory>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // allocate an actor.
      actor = new gear::Actor(scope);

      // attach the actor to the scope.
      if (actor->Attach() == elle::StatusError)
	escape("unable to attach the actor to the scope");

      // return the identifier.
      identifier = actor->identifier;

      // waive the actor.
      waive(actor);

      // apply the create automaton on the context.
      if (automaton::Directory::Create(*context) == elle::StatusError)
	escape("unable to create the directory");

      leave();
    }

    ///
    /// this method loads the directory referenced through the given
    /// chemin.
    ///
    elle::Status	Directory::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::Directory*	context;
      gear::Actor*	actor;
      nucleus::Location	location;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
	escape("unable to acquire the scope");

      // retrieve the context.
      if (scope->Use<gear::NatureDirectory>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // allocate an actor.
      actor = new gear::Actor(scope);

      // attach the actor to the scope.
      if (actor->Attach() == elle::StatusError)
	escape("unable to attach the actor to the scope");

      // return the identifier.
      identifier = actor->identifier;

      // waive the actor.
      waive(actor);

      // locate the object based on the chemin.
      if (chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the directory");

      // apply the load automaton on the context.
      if (automaton::Directory::Load(*context,
				     location) == elle::StatusError)
	escape("unable to load the directory");

      leave();
    }

    ///
    /// this method locks the given directory.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status	Directory::Lock(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Lock()\n");

      // XXX

      leave();
    }

    ///
    /// this method releases a previously locked directory.
    ///
    elle::Status	Directory::Release(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Release()\n");

      // XXX

      leave();
    }

    ///
    /// this method adds an entry to the given directory.
    ///
    elle::Status	Directory::Add(
			  const gear::Identifier&		parent,
			  const path::Slice&			name,
			  const gear::Identifier&		child)
    {
      gear::Actor*	actor;
      gear::Directory*	directory;
      gear::Object*	object;
      nucleus::Address	address;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Add()\n");

      // select the actor.
      if (gear::Actor::Select(child, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureObject>(object) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // keep the address.
      address = object->location.address;

      // select the actor.
      if (gear::Actor::Select(parent, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(directory) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the add automaton on the context.
      if (automaton::Directory::Add(*directory,
				    name,
				    address) == elle::StatusError)
	escape("unable to add the directory entry");

      // set the actor's state.
      actor->state = gear::Actor::StateUpdated;

      leave();
    }

    ///
    /// this method returns the directory entry associated with the
    /// given name.
    ///
    elle::Status	Directory::Lookup(
			  const gear::Identifier&		identifier,
			  const path::Slice&			name,
			  nucleus::Entry*&			entry)
    {
      gear::Actor*	actor;
      gear::Directory*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Lookup()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the lookup automaton on the context.
      if (automaton::Directory::Lookup(*context,
				       name,
				       entry) == elle::StatusError)
	escape("unable to lookup the directory entry");

      leave();
    }

    ///
    /// this method returns a set [offset, offset + size[ of entries
    /// (name, address) from the directory identified by _identifier_.
    ///
    elle::Status	Directory::Consult(
			  const gear::Identifier&		identifier,
			  const nucleus::Offset&		offset,
			  const nucleus::Offset&		size,
			  nucleus::Range<nucleus::Entry>&	range)
    {
      gear::Actor*	actor;
      gear::Directory*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Consult()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the consult automaton on the context.
      if (automaton::Directory::Consult(*context,
					offset,
					size,
					range) == elle::StatusError)
	escape("unable to consult the directory entries");

      leave();
    }

    ///
    /// this method renames a directory entry.
    ///
    elle::Status	Directory::Rename(
			  const gear::Identifier&		identifier,
			  const path::Slice&			from,
			  const path::Slice&			to)
    {
      gear::Actor*	actor;
      gear::Directory*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Rename()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the rename automaton on the context.
      if (automaton::Directory::Rename(*context,
				       from,
				       to) == elle::StatusError)
	escape("unable to rename the directory entry");

      // set the actor's state.
      actor->state = gear::Actor::StateUpdated;

      leave();
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status	Directory::Remove(
			  const gear::Identifier&		identifier,
			  const path::Slice&			name)
    {
      gear::Actor*	actor;
      gear::Directory*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Remove()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the remove automaton on the context.
      if (automaton::Directory::Remove(*context,
				       name) == elle::StatusError)
	escape("unable to remove the directory entry");

      // set the actor's state.
      actor->state = gear::Actor::StateUpdated;

      leave();
    }

    ///
    /// this method discards the scope along with the possible
    /// modifications having been performed.
    ///
    elle::Status	Directory::Discard(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationDiscard) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(gear::OperationDiscard) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // record the scope in the journal.
      if (journal::Journal::Record(actor->scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      leave();
    }

    ///
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status	Directory::Store(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Directory*	context;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationStore) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(gear::OperationStore) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the store automaton on the context.
      if (automaton::Directory::Store(*context) == elle::StatusError)
	escape("unable to store the object");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // record the scope in the journal.
      if (journal::Journal::Record(actor->scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      leave();
    }

    ///
    /// this method destroys a directory.
    ///
    elle::Status	Directory::Destroy(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Directory*	context;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationDestroy) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(gear::OperationDestroy) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureDirectory>(context) ==
	  elle::StatusError)
	escape("unable to retrieve the context");

      // apply the destroy automaton on the context.
      if (automaton::Directory::Destroy(*context) == elle::StatusError)
	escape("unable to destroy the object");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // record the scope in the journal.
      if (journal::Journal::Record(actor->scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      leave();
    }

    ///
    /// this method purges a directory i.e removes all the blocks of all
    /// the versions associated with this directory.
    ///
    elle::Status	Directory::Purge(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Directory::Purge()\n");

      // XXX

      leave();
    }

  }
}
