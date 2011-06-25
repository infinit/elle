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
// updated       julien quintard   [sat jun 25 16:29:30 2011]
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

#include <etoile/automaton/Directory.hh>

#include <etoile/journal/Journal.hh>

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

      enter(instance(scope));

      printf("[XXX] Directory::Create()\n");

      // allocate the scope.
      scope = new gear::Scope(gear::NatureDirectory);

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the create automaton on the context.
      if (automaton::Directory::Create(*context) == elle::StatusError)
	escape("unable to create the directory");

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
    /// this method loads the directory referenced through the given
    /// chemin.
    ///
    elle::Status	Directory::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::Directory*	context;
      nucleus::Location	location;

      enter(instance(scope));

      printf("[XXX] Directory::Load()\n");

      // allocate the scope.
      scope = new gear::Scope(gear::NatureDirectory);

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // locate the object based on the chemin.
      if (chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the directory");

      // apply the load automaton on the context.
      if (automaton::Directory::Load(*context,
				     location) == elle::StatusError)
	escape("unable to load the directory");

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
    /// this method locks the given directory.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status	Directory::Lock(
			  const gear::Identifier&)
    {
      enter();

      printf("[XXX] Directory::Lock()\n");

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

      printf("[XXX] Directory::Release()\n");

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
      gear::Scope*	scope;
      gear::Directory*	directory;
      gear::Object*	object;
      nucleus::Address	address;

      enter();

      printf("[XXX] Directory::Add()\n");

      // select the scope associated with the child identifier.
      if (gear::Gear::Select(child, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(object) == elle::StatusError)
	escape("unable to retrieve the context");

      // keep the address.
      address = object->location.address;

      // select the scope associated with the parent identifier.
      if (gear::Gear::Select(parent, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(directory) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the add automaton on the context.
      if (automaton::Directory::Add(*directory,
				    name,
				    address) == elle::StatusError)
	escape("unable to add the directory entry");

      leave();
    }

    ///
    /// this method returns the directory entry associated with the
    /// given name.
    ///
    elle::Status	Directory::Lookup(
			  const gear::Identifier&		identifier,
			  const path::Slice&			name,
			  nucleus::Entry&			entry)
    {
      gear::Scope*	scope;
      gear::Directory*	context;

      enter();

      printf("[XXX] Directory::Lookup()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
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
      gear::Scope*	scope;
      gear::Directory*	context;

      enter();

      printf("[XXX] Directory::Consult()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
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
      gear::Scope*	scope;
      gear::Directory*	context;

      enter();

      printf("[XXX] Directory::Rename()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the rename automaton on the context.
      if (automaton::Directory::Rename(*context,
				       from,
				       to) == elle::StatusError)
	escape("unable to rename the directory entry");

      leave();
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status	Directory::Remove(
			  const gear::Identifier&		identifier,
			  const path::Slice&			name)
    {
      gear::Scope*	scope;
      gear::Directory*	context;

      enter();

      printf("[XXX] Directory::Remove()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the remove automaton on the context.
      if (automaton::Directory::Remove(*context,
				       name) == elle::StatusError)
	escape("unable to remove the directory entry");

      leave();
    }

    ///
    /// this method discards the scope along with the possible
    /// modifications having been performed.
    ///
    elle::Status	Directory::Discard(
			  const gear::Identifier&		identifier)
    {
      gear::Scope*	scope;

      enter();

      printf("[XXX] Directory::Discard()\n");

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
    elle::Status	Directory::Store(
			  const gear::Identifier&		identifier)
    {
      gear::Scope*	scope;
      gear::Directory*	context;

      enter();

      printf("[XXX] Directory::Store()\n");

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
      if (automaton::Directory::Store(*context) == elle::StatusError)
	escape("unable to store the directory");

      // record the scope in the journal.
      if (journal::Journal::Record(scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      leave();
    }

    ///
    /// this method destroys a directory.
    ///
    elle::Status	Directory::Destroy(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Directory::Destroy()\n");

      // XXX

      leave();
    }

    ///
    /// this method purges a directory i.e removes all the blocks of all
    /// the versions associated with this directory.
    ///
    elle::Status	Directory::Purge(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] Directory::Purge()\n");

      // XXX

      leave();
    }

  }
}
