//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Access.cc
//
// created       julien quintard   [wed mar 31 19:26:06 2010]
// updated       julien quintard   [wed aug  3 23:02:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Access.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Access.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns the caller the access record associated with
    /// the given subject.
    ///
    elle::Status	Access::Lookup(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject,
			  nucleus::Record*&			record)
    {
      gear::Actor*	actor;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Access::Lookup()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureObject>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the lookup automaton on the context.
      if (automaton::Access::Lookup(*context,
				    subject,
				    record) == elle::StatusError)
	escape("unable to lookup the access record");

      leave();
    }

    ///
    /// this method returns a subset of the object's access access list.
    ///
    elle::Status	Access::Consult(
			  const gear::Identifier&		identifier,
			  const nucleus::Index&			index,
			  const nucleus::Size&			size,
			  nucleus::Range<nucleus::Record>&	range)
    {
      gear::Actor*	actor;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Access::Consult()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureObject>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the consult automaton on the context.
      if (automaton::Access::Consult(*context,
				     index,
				     size,
				     range) == elle::StatusError)
	escape("unable to consult the access records");

      leave();
    }

    ///
    /// this method grants the given access permissions to the subject.
    ///
    elle::Status	Access::Grant(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject,
			  const nucleus::Permissions&		permissions)
    {
      gear::Actor*	actor;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Access::Grant()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureObject>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the grant automaton on the context.
      if (automaton::Access::Grant(*context,
				   subject,
				   permissions) == elle::StatusError)
	escape("unable to grant access to the subject");

      leave();
    }

    ///
    /// this method removes the user's permissions from the access control
    /// list.
    ///
    elle::Status	Access::Revoke(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject)
    {
      gear::Actor*	actor;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Access::Revoke()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use<gear::NatureObject>(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the revoke automaton on the context.
      if (automaton::Access::Revoke(*context,
				    subject) == elle::StatusError)
	escape("unable to revoke the subject's access permissions");

      leave();
    }

  }
}
