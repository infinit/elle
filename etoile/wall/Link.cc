//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Link.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [sun jul 31 11:31:34 2011]
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

#include <etoile/journal/Journal.hh>

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

      enter(instance(actor));

      printf("[XXX] Link::Create()\n");

      // acquire the scope.
      if (gear::Scope::Supply(scope) == elle::StatusError)
	escape("unable to supply the scope");

      // retrieve the context.
      if (scope->Use<gear::NatureLink>(context) == elle::StatusError)
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
      if (automaton::Link::Create(*context) == elle::StatusError)
	escape("unable to create the link");

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
      nucleus::Location	location;

      enter(instance(actor));

      printf("[XXX] Link::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
	escape("unable to acquire the scope");

      // retrieve the context.
      if (scope->Use<gear::NatureLink>(context) == elle::StatusError)
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
	escape("unable to locate the link");

      // apply the load automaton on the context.
      if (automaton::Link::Load(*context,
				location) == elle::StatusError)
	escape("unable to load the link");

      leave();
    }

    ///
    /// this application locks the given link.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	Link::Lock(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Link::Lock()\n");

      // XXX

      leave();
    }

    ///
    /// this method releases a previously locked link.
    ///
    elle::Status	Link::Release(
			  const gear::Identifier&		identifer)
    {
      enter();

      printf("[XXX] Link::Release()\n");

      // XXX

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
      gear::Link*	context;

      enter();

      printf("[XXX] Link::Bind()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureLink>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the bind automaton on the context.
      if (automaton::Link::Bind(*context,
				way) == elle::StatusError)
	escape("unable to bind the link");

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
      gear::Link*	context;

      enter();

      printf("[XXX] Link::Resolve()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureLink>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the resolve automaton on the context.
      if (automaton::Link::Resolve(*context,
				   way) == elle::StatusError)
	escape("unable to resolve the link");

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

      enter(instance(actor));

      printf("[XXX] Link::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(gear::OperationDiscard) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // relinquish the scope.
      if (gear::Scope::Relinquish(actor->scope) == elle::StatusError)
	escape("unable to relinquish the scope");

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
    elle::Status	Link::Store(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Link*	context;

      enter(instance(actor));

      printf("[XXX] Link::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(gear::OperationStore) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureLink>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the store automaton on the context.
      if (automaton::Link::Store(*context) == elle::StatusError)
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
    /// this method destroys a link.
    ///
    elle::Status	Link::Destroy(
			  const gear::Identifier&		identifier)
    {
      gear::Actor*	actor;
      gear::Link*	context;

      enter(instance(actor));

      printf("[XXX] Link::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate(gear::OperationDestroy) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureLink>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the destroy automaton on the context.
      if (automaton::Link::Destroy(*context) == elle::StatusError)
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
    /// this method purges a link i.e removes all the blocks of all
    /// the versions associated with this link.
    ///
    elle::Status	Link::Purge(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Link::Purge()\n");

      // XXX

      leave();
    }

  }
}
