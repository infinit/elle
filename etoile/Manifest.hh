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
// updated       julien quintard   [thu may 12 00:04:37 2011]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Way.hh>
#include <etoile/path/Slice.hh>

#include <etoile/context/Identifier.hh>

#include <etoile/wall/Status.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace etoile
{

  ///
  /// XXX
  ///
  extern const elle::Character	Component[];

  ///
  /// XXX
  ///
  const elle::Natural32		Tags = 200;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(etoile::Component, etoile::Tags, elle::Component);

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
      TagOk = elle::Range<Component>::First,
      TagIdentifier,

      // wall
      TagWallIdentify,
      TagWallChallenge,
      TagWallAuthenticate,
      TagWallConnect,

      // object
      TagObjectLoad,
      TagObjectInformation,
      TagObjectStatus,
      TagObjectDiscard,
      TagObjectStore,

      // directory
      TagDirectoryCreate,
      TagDirectoryLoad,
      TagDirectoryAdd,
      TagDirectoryLookup,
      TagDirectoryEntry,
      TagDirectoryConsult,
      TagDirectoryRange,
      TagDirectoryRename,
      TagDirectoryRemove,
      TagDirectoryDiscard,
      TagDirectoryStore,
      TagDirectoryDestroy,

      // file
      TagFileCreate,
      TagFileLoad,
      TagFileWrite,
      TagFileRead,
      TagFileRegion,
      TagFileAdjust,
      TagFileDiscard,
      TagFileStore,
      TagFileDestroy,

      // link
      TagLinkCreate,
      TagLinkLoad,
      TagLinkBind,
      TagLinkResolve,
      TagLinkWay,
      TagLinkDiscard,
      TagLinkStore,
      TagLinkDestroy,

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
      TagAttributesSet,
      TagAttributesGet,
      TagAttributesTrait,
      TagAttributesFetch,
      TagAttributesRange,
      TagAttributesOmit,
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

// etoile
outward(etoile::TagOk,
	parameters());
outward(etoile::TagIdentifier,
	parameters(const etoile::context::Identifier));

// wall
inward(etoile::TagWallIdentify,
       parameters(const elle::PublicKey));
outward(etoile::TagWallChallenge,
	parameters(const elle::Code));
inward(etoile::TagWallAuthenticate,
       parameters(const elle::Digest));
inward(etoile::TagWallConnect,
       parameters(const elle::String,
		  const nucleus::Network));

// object
inward(etoile::TagObjectLoad,
       parameters(const etoile::path::Way));
inward(etoile::TagObjectInformation,
       parameters(const etoile::context::Identifier));
outward(etoile::TagObjectStatus,
	parameters(const etoile::wall::Status));
inward(etoile::TagObjectDiscard,
       parameters(const etoile::context::Identifier));
inward(etoile::TagObjectStore,
       parameters(const etoile::context::Identifier));
// XXX + lock/release

// directory
inward(etoile::TagDirectoryCreate,
       parameters());
inward(etoile::TagDirectoryLoad,
       parameters(const etoile::path::Way));
inward(etoile::TagDirectoryAdd,
       parameters(const etoile::context::Identifier,
		  const etoile::path::Slice,
		  const etoile::context::Identifier));
inward(etoile::TagDirectoryLookup,
       parameters(const etoile::context::Identifier,
		  const etoile::path::Slice));
outward(etoile::TagDirectoryEntry,
	parameters(const nucleus::Entry));
inward(etoile::TagDirectoryConsult,
       parameters(const etoile::context::Identifier,
		  const nucleus::Index,
		  const nucleus::Size));
outward(etoile::TagDirectoryRange,
	parameters(const nucleus::Range<nucleus::Entry>));
inward(etoile::TagDirectoryRename,
       parameters(const etoile::context::Identifier,
		  const etoile::path::Slice,
		  const etoile::path::Slice));
inward(etoile::TagDirectoryRemove,
       parameters(const etoile::context::Identifier,
		  const etoile::path::Slice));
inward(etoile::TagDirectoryDiscard,
       parameters(const etoile::context::Identifier));
inward(etoile::TagDirectoryStore,
       parameters(const etoile::context::Identifier));
inward(etoile::TagDirectoryDestroy,
       parameters(const etoile::context::Identifier));
// XXX + lock/release

// file
inward(etoile::TagFileCreate,
       parameters());
inward(etoile::TagFileLoad,
       parameters(const etoile::path::Way));
inward(etoile::TagFileWrite,
       parameters(const etoile::context::Identifier,
		  const nucleus::Offset,
		  const elle::Region));
inward(etoile::TagFileRead,
       parameters(const etoile::context::Identifier,
		  const nucleus::Offset,
		  const nucleus::Size));
outward(etoile::TagFileRegion,
	parameters(const elle::Region));
inward(etoile::TagFileAdjust,
       parameters(const etoile::context::Identifier,
		  const nucleus::Size));
inward(etoile::TagFileDiscard,
       parameters(const etoile::context::Identifier));
inward(etoile::TagFileStore,
       parameters(const etoile::context::Identifier));
inward(etoile::TagFileDestroy,
       parameters(const etoile::context::Identifier));
// XXX + lock/release

// link
inward(etoile::TagLinkCreate,
       parameters());
inward(etoile::TagLinkLoad,
       parameters(const etoile::path::Way));
inward(etoile::TagLinkBind,
       parameters(const etoile::context::Identifier,
		  const etoile::path::Way));
inward(etoile::TagLinkResolve,
       parameters(const etoile::context::Identifier));
outward(etoile::TagLinkWay,
	parameters(const etoile::path::Way));
inward(etoile::TagLinkDiscard,
       parameters(const etoile::context::Identifier));
inward(etoile::TagLinkStore,
       parameters(const etoile::context::Identifier));
inward(etoile::TagLinkDestroy,
       parameters(const etoile::context::Identifier));
// XXX + lock/release

// access
inward(etoile::TagAccessLookup,
       parameters(const etoile::context::Identifier,
		  const nucleus::Subject));
outward(etoile::TagAccessRecord,
	parameters(const nucleus::Record));
inward(etoile::TagAccessConsult,
       parameters(const etoile::context::Identifier,
		  const nucleus::Index,
		  const nucleus::Size));
outward(etoile::TagAccessRange,
	parameters(const nucleus::Range<nucleus::Record>));
inward(etoile::TagAccessGrant,
       parameters(const etoile::context::Identifier,
		  const nucleus::Subject,
		  const nucleus::Permissions));
inward(etoile::TagAccessUpdate,
       parameters(const etoile::context::Identifier,
		  const nucleus::Subject,
		  const nucleus::Permissions));
inward(etoile::TagAccessBlock,
       parameters(const etoile::context::Identifier,
		  const nucleus::Subject));
inward(etoile::TagAccessRevoke,
       parameters(const etoile::context::Identifier,
		  const nucleus::Subject));

// attributes
inward(etoile::TagAttributesSet,
       parameters(const etoile::context::Identifier,
		  const elle::String,
		  const elle::String));
inward(etoile::TagAttributesGet,
       parameters(const etoile::context::Identifier,
		  const elle::String));
outward(etoile::TagAttributesTrait,
	parameters(const nucleus::Trait));
inward(etoile::TagAttributesFetch,
       parameters(const etoile::context::Identifier));
outward(etoile::TagAttributesRange,
	parameters(const nucleus::Range<nucleus::Trait>));
inward(etoile::TagAttributesOmit,
       parameters(const etoile::context::Identifier,
		  const elle::String));

//
// ---------- dependencies ----------------------------------------------------
//

#include <agent/Manifest.hh>

#endif
