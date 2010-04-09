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
// updated       julien quintard   [wed apr  7 17:24:09 2010]
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
      TagBoolean,

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
      TagDirectoryExist,
      TagDirectoryLookup,
      TagDirectoryEntry,
      TagDirectoryConsult,
      TagDirectorySet,
      TagDirectoryRename,
      TagDirectoryStore,

      // access
      TagAccessExist,
      TagAccessLookup,
      TagAccessRecord,
      TagAccessConsult,
      TagAccessRange,
      TagAccessGrant,
      TagAccessUpdate,
      TagAccessBlock,
      TagAccessRevoke,
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
outward(::etoile::TagBoolean,
	parameters(const ::elle::core::Boolean));

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
       parameters(const ::etoile::path::Way));
inward(::etoile::TagDirectoryExist,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::path::Slice));
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
inward(::etoile::TagDirectoryStore,
       parameters(const ::etoile::context::Identifier));
// XXX + lock/release

// access
inward(::etoile::TagAccessExist,
       parameters(const ::etoile::context::Identifier,
		  const ::etoile::kernel::Subject));
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

//
// ---------- dependencies ----------------------------------------------------
//

#include <agent/Manifest.hh>

#endif
