//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/Manifest.hh
//
// created       julien quintard   [thu mar  4 17:35:00 2010]
// updated       julien quintard   [fri apr 16 11:52:53 2010]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/path/Way.hh>
#include <etoile/path/Slice.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/wall/State.hh>

#include <etoile/kernel/Size.hh>
#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Range.hh>
#include <etoile/kernel/Set.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace etoile
{

  ///
  /// XXX
  ///
  extern const Character	Component[];

  ///
  /// XXX
  ///
  const Natural32		Tags = 100;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(::etoile::Component, ::etoile::Tags, ::elle::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace etoile
{

  //
  // enumerations
  //
  enum Tag
    {
      // etoile
      TagOk = Range<Component>::First,
      TagIdentifier,

      // wall
      TagWallIdentify,
      TagWallChallenge,
      TagWallAuthenticate,
      TagWallConnect,

      // object
      TagObjectLoad,
      TagObjectInformation,
      TagObjectState,
      TagObjectStore,

      // directory
      TagDirectoryLoad,
      TagDirectoryCreate,
      TagDirectoryAdd,
      TagDirectoryLookup,
      TagDirectoryEntry,
      TagDirectoryConsult,
      TagDirectorySet,
      TagDirectoryRename,
      TagDirectoryRemove,
      TagDirectoryStore,
      TagDirectoryDestroy,

      // access
      TagAccessLookup,
      TagAccessRecord,
      TagAccessConsult,
      TagAccessRange,
      TagAccessGrant,
      TagAccessUpdate,
      TagAccessBlock,
      TagAccessRevoke,

      // attributes
      TagAttributesAdd,
      TagAttributesLookup,
      TagAttributesTrait,
      TagAttributesConsult,
      TagAttributesCollection,
      TagAttributesUpdate,
      TagAttributesRemove,
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

// etoile
outward(::etoile::TagOk,
	parameters());
outward(::etoile::TagIdentifier,
	parameters(const ::etoile::context::Identifier));

// wall
inward(::etoile::TagWallIdentify,
       parameters(const ::elle::cryptography::PublicKey));
outward(::etoile::TagWallChallenge,
	parameters(const ::elle::cryptography::Code));
inward(::etoile::TagWallAuthenticate,
       parameters(const ::elle::cryptography::Digest));
inward(::etoile::TagWallConnect,
       parameters(const ::elle::core::String));

// object
inward(::etoile::TagObjectLoad,
       parameters(const ::etoile::path::Way));
inward(::etoile::TagObjectInformation,
       parameters(const ::etoile::context::Identifier));
outward(::etoile::TagObjectState,
	parameters(const ::etoile::wall::State));
inward(::etoile::TagObjectStore,
       parameters(const ::etoile::context::Identifier));
// XXX + lock/release

// directory
inward(::etoile::TagDirectoryLoad,
       parameters(const ::etoile::path::Way));
inward(::etoile::TagDirectoryCreate,
       parameters());
inward(::etoile::TagDirectoryAdd,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::path::Slice,
		  const ::etoile::context::Identifier));
inward(::etoile::TagDirectoryLookup,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::path::Slice));
outward(::etoile::TagDirectoryEntry,
	parameters(const ::etoile::kernel::Entry));
inward(::etoile::TagDirectoryConsult,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Index,
		  const ::etoile::kernel::Size));
outward(::etoile::TagDirectorySet,
	parameters(const ::etoile::kernel::Set));
inward(::etoile::TagDirectoryRename,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::path::Slice,
		  const ::etoile::path::Slice));
inward(::etoile::TagDirectoryRemove,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::path::Slice));
inward(::etoile::TagDirectoryStore,
       parameters(const ::etoile::context::Identifier));
inward(::etoile::TagDirectoryDestroy,
       parameters(const ::etoile::context::Identifier));
// XXX + lock/release

// access
inward(::etoile::TagAccessLookup,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Subject));
outward(::etoile::TagAccessRecord,
	parameters(const ::etoile::kernel::Record));
inward(::etoile::TagAccessConsult,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Index,
		  const ::etoile::kernel::Size));
outward(::etoile::TagAccessRange,
	parameters(const ::etoile::kernel::Range));
inward(::etoile::TagAccessGrant,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Subject,
		  const ::etoile::kernel::Permissions));
inward(::etoile::TagAccessUpdate,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Subject,
		  const ::etoile::kernel::Permissions));
inward(::etoile::TagAccessBlock,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Subject));
inward(::etoile::TagAccessRevoke,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Subject));

// attributes
inward(::etoile::TagAttributesAdd,
       parameters(const ::etoile::context::Identifier,
		  const ::elle::core::String,
		  const ::elle::core::String));
inward(::etoile::TagAttributesLookup,
       parameters(const ::etoile::context::Identifier,
		  const ::elle::core::String));
outward(::etoile::TagAttributesTrait,
	parameters(const ::etoile::kernel::Trait));
inward(::etoile::TagAttributesConsult,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Index,
		  const ::etoile::kernel::Size));
outward(::etoile::TagAttributesCollection,
	parameters(const ::etoile::kernel::Collection));
inward(::etoile::TagAttributesUpdate,
       parameters(const ::etoile::context::Identifier,
		  const ::elle::core::String,
		  const ::elle::core::String));
inward(::etoile::TagAttributesRemove,
       parameters(const ::etoile::context::Identifier,
		  const ::elle::core::String));

//
// ---------- dependencies ----------------------------------------------------
//

#include <agent/Manifest.hh>

#endif
