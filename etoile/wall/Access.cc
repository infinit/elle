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
// updated       julien quintard   [thu jun 23 15:17:58 2011]
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
			  nucleus::Record&			record)
    {
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Access::Lookup()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // apply the lookup automaton on the context.
      if (automaton::Access::Lookup(scope->context,
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
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Access::Consult()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // apply the consult automaton on the context.
      if (automaton::Access::Consult(scope->context,
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
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Access::Grant()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // apply the grant automaton on the context.
      if (automaton::Access::Grant(scope->context,
				   subject,
				   permissions) == elle::StatusError)
	escape("unable to grant access to the subject");

      leave();
    }

    ///
    /// this method updates the permissions associated with the subject.
    ///
    elle::Status	Access::Update(
			  const gear::Identifier&		identifier,
			  const nucleus::Subject&		subject,
			  const nucleus::Permissions&		permissions)
    {
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Access::Update()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // apply the update automaton on the context.
      if (automaton::Access::Update(scope->context,
				    subject,
				    permissions) == elle::StatusError)
	escape("unable to update the subject's access permissions");

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
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Access::Revoke()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // apply the revoke automaton on the context.
      if (automaton::Access::Revoke(scope->context,
				    subject) == elle::StatusError)
	escape("unable to revoke the subject's access permissions");

      leave();
    }

  }
}
