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
// updated       julien quintard   [sat jun 25 14:36:08 2011]
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

      enter(instance(scope));

      printf("[XXX] Link::Create()\n");

      // allocate the scope.
      scope = new gear::Scope(gear::NatureLink);

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the create automaton on the context.
      if (automaton::Link::Create(*context) == elle::StatusError)
	escape("unable to create the link");

      // export the scope.
      if (scope->Export() == elle::StatusError)
	escape("unable to export the context");

      // return the identifier.
      identifier = scope->identifier;

      // waive.
      waive(scope);

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
      nucleus::Location	location;

      enter(instance(scope));

      printf("[XXX] Link::Load()\n");

      // allocate the scope.
      scope = new gear::Scope(gear::NatureLink);

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // locate the object based on the chemin.
      if (chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the link");

      // apply the load automaton on the context.
      if (automaton::Link::Load(*context,
				location) == elle::StatusError)
	escape("unable to load the link");

      // export the scope.
      if (scope->Export() == elle::StatusError)
	escape("unable to export the context");

      // return the identifier.
      identifier = scope->identifier;

      // waive.
      waive(scope);

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
      gear::Scope*	scope;
      gear::Link*	context;

      enter();

      printf("[XXX] Link::Bind()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
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
      gear::Scope*	scope;
      gear::Link*	context;

      enter();

      printf("[XXX] Link::Resolve()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
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
      gear::Scope*	scope;

      enter();

      printf("[XXX] Link::Discard()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // import the scope, making it unusable through its identifier.
      if (scope->Import() == elle::StatusError)
	escape("unable to import the scope");

      // delete the scope.
      delete scope;

      leave();
    }

    ///
    /// this method closes the scope and places it in the journal for
    /// the modifications to be published in the storage layer.
    ///
    elle::Status	Link::Store(
			  const gear::Identifier&		identifier)
    {
      gear::Scope*	scope;
      gear::Link*	context;

      enter();

      printf("[XXX] Link::Store()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // import the scope, making it unusable through its identifier.
      if (scope->Import() == elle::StatusError)
	escape("unable to import the scope");

      // apply the store automaton on the context.
      if (automaton::Link::Store(*context) == elle::StatusError)
	escape("unable to store the link");

      // record the scope in the journal.
      if (journal::Journal::Record(scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      leave();
    }

    ///
    /// this method destroys a link.
    ///
    elle::Status	Link::Destroy(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Link::Destroy()\n");

      // XXX

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
