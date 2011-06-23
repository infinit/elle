//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Object.cc
//
// created       julien quintard   [wed mar  3 20:50:57 2010]
// updated       julien quintard   [thu jun 23 14:33:58 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Object.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Object.hh>

#include <etoile/miscellaneous/Information.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method loads the object identified by _chemin_ into a new
    /// scope and returns the scope's identifier so that subsequent
    /// operations can be carried out.
    ///
    elle::Status	Object::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope<gear::Object>*	scope;
      nucleus::Location			location;

      enter(instance(scope));

      printf("[XXX] Object::Load()\n");

      // allocate the scope.
      scope = new gear::Scope<gear::Object>;

      // locate the object based on the chemin.
      if (chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the directory");

      // apply the load automaton on the context.
      if (automaton::Object::Load(scope->context,
				  location) == elle::StatusError)
	escape("unable to load the object");

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
    /// this method locks the object.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status	Object::Lock(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Object::Lock()\n");

      // XXX

      leave();
    }

    ///
    /// this method releases a previously locked object.
    ///
    elle::Status	Object::Release(
			  const gear::Identifier&		identifer)
    {
      enter();

      printf("[XXX] Object::Release()\n");

      // XXX

      leave();
    }

    ///
    /// this method returns general information regarding the identified
    /// object.
    ///
    elle::Status	Object::Information(
			  const gear::Identifier&		identifier,
			  miscellaneous::Information&		information)
    {
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Object::Information()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // apply the information automaton on the context.
      if (automaton::Object::Information(scope->context,
					 information) == elle::StatusError)
	escape("unable to retrieve general information on the object");

      leave();
    }

    ///
    /// this method discards the scope hence potentially ignoring some
    /// modifications.
    ///
    elle::Status	Object::Discard(
			  const gear::Identifier&		identifier)
    {
      gear::Scope<gear::Object>*	scope;

      enter();

      printf("[XXX] Object::Discard()\n");

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
    /// this method commits the pending modifications by placing the
    /// scope in the journal.
    ///
    elle::Status	Object::Store(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Object::Store()\n");

      // XXX

      leave();
    }

    ///
    /// this method destroys an object.
    ///
    /// this method should be used with great care since, not knowing the
    /// object's genre, the data blocks will not be removed. therefore,
    /// the genre-specific Destroy() method should always be preferred.
    ///
    elle::Status	Object::Destroy(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Object::Destroy()\n");

      // XXX

      leave();
    }

    ///
    /// this method purges an object i.e removes all the blocks of all
    /// the versions associated with this object.
    ///
    /// this method should be used with great care since, not knowing the
    /// object's genre, the data blocks will not be removed. therefore,
    /// the genre-specific Destroy() method should always be preferred.
    ///
    elle::Status	Object::Purge(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Object::Purge()\n");

      // XXX

      leave();
    }

  }
}
