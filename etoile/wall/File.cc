//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/File.cc
//
// created       julien quintard   [fri aug 14 16:34:43 2009]
// updated       julien quintard   [wed jul  6 11:33:37 2011]
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

      enter(instance(scope));

      printf("[XXX] File::Create()\n");

      // allocate the scope.
      scope = new gear::Scope(gear::NatureFile);

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the create automaton on the context.
      if (automaton::File::Create(*context) == elle::StatusError)
	escape("unable to create the file");

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
    /// this method loads a file object given its chemin and initializes
    /// an associated context.
    ///
    elle::Status	File::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::File*	context;
      nucleus::Location	location;

      enter(instance(scope));

      printf("[XXX] File::Load()\n");

      // allocate the scope.
      scope = new gear::Scope(gear::NatureFile);

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // locate the object based on the chemin.
      if (chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the file");

      // apply the load automaton on the context.
      if (automaton::File::Load(*context,
				location) == elle::StatusError)
	escape("unable to load the file");

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
    /// this application locks the given file.
    ///
    /// the method returns true if the lock has been acquired or false
    /// otherwise.
    ///
    elle::Status	File::Lock(
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] File::Lock()\n");

      // XXX

      leave();
    }

    ///
    /// this method releases a previously locked file.
    ///
    elle::Status	File::Release(
			  const gear::Identifier&		identifer)
    {
      enter();

      printf("[XXX] File::Release()\n");

      // XXX

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
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      printf("[XXX] File::Write()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the write automaton on the context.
      if (automaton::File::Write(*context,
				 offset,
				 region) == elle::StatusError)
	escape("unable to write the file");

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
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      printf("[XXX] File::Read()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
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
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      printf("[XXX] File::Adjust()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the adjust automaton on the context.
      if (automaton::File::Adjust(*context,
				  size) == elle::StatusError)
	escape("unable to adjust the file's size");

      leave();
    }

    ///
    /// this method discards the scope, potentially ignoring the
    /// performed modifications.
    ///
    elle::Status	File::Discard(
			  const gear::Identifier&		identifier)
    {
      gear::Scope*	scope;

      enter();

      printf("[XXX] File::Discard()\n");

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
    elle::Status	File::Store(
			  const gear::Identifier&		identifier)
    {
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      printf("[XXX] File::Store()\n");

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
      if (automaton::File::Store(*context) == elle::StatusError)
	escape("unable to store the file");

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
      gear::Scope*	scope;
      gear::File*	context;

      enter();

      printf("[XXX] File::Destroy()\n");

      // select the scope associated with the identifier.
      if (gear::Gear::Select(identifier, scope) == elle::StatusError)
	escape("unable to select the scope");

      // retrieve the context.
      if (scope->context->Cast(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // import the scope, making it unusable through its identifier.
      if (scope->Import() == elle::StatusError)
	escape("unable to import the scope");

      // apply the destroy automaton on the context.
      if (automaton::File::Destroy(*context) == elle::StatusError)
	escape("unable to destroy the file");

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
			  const gear::Identifier&		identifier)
    {
      enter();

      printf("[XXX] File::Purge()\n");

      // XXX

      leave();
    }

  }
}
