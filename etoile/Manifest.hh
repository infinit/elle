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
// updated       julien quintard   [mon may 24 22:22:25 2010]
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

#include <etoile/wall/Status.hh>

#include <etoile/kernel/Size.hh>
#include <etoile/kernel/Index.hh>
#include <etoile/kernel/Offset.hh>
#include <etoile/kernel/Range.hh>
#include <etoile/kernel/Entry.hh>
#include <etoile/kernel/Record.hh>
#include <etoile/kernel/Trait.hh>

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
  const elle::Natural32		Tags = 100;

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
       parameters(const elle::String));

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
	parameters(const etoile::kernel::Entry));
inward(etoile::TagDirectoryConsult,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Index,
		  const etoile::kernel::Size));
outward(etoile::TagDirectoryRange,
	parameters(const etoile::kernel::Range<etoile::kernel::Entry>));
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
		  const etoile::kernel::Offset,
		  const elle::Region));
inward(etoile::TagFileRead,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Offset,
		  const etoile::kernel::Size));
outward(etoile::TagFileRegion,
	parameters(const elle::Region));
inward(etoile::TagFileAdjust,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Size));
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
		  const etoile::kernel::Subject));
outward(etoile::TagAccessRecord,
	parameters(const etoile::kernel::Record));
inward(etoile::TagAccessConsult,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Index,
		  const etoile::kernel::Size));
outward(etoile::TagAccessRange,
	parameters(const etoile::kernel::Range<etoile::kernel::Record>));
inward(etoile::TagAccessGrant,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Subject,
		  const etoile::kernel::Permissions));
inward(etoile::TagAccessUpdate,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Subject,
		  const etoile::kernel::Permissions));
inward(etoile::TagAccessBlock,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Subject));
inward(etoile::TagAccessRevoke,
       parameters(const etoile::context::Identifier,
		  const etoile::kernel::Subject));

// attributes
inward(etoile::TagAttributesSet,
       parameters(const etoile::context::Identifier,
		  const elle::String,
		  const elle::String));
inward(etoile::TagAttributesGet,
       parameters(const etoile::context::Identifier,
		  const elle::String));
outward(etoile::TagAttributesTrait,
	parameters(const etoile::kernel::Trait));
inward(etoile::TagAttributesFetch,
       parameters(const etoile::context::Identifier));
outward(etoile::TagAttributesRange,
	parameters(const etoile::kernel::Range<etoile::kernel::Trait>));
inward(etoile::TagAttributesOmit,
       parameters(const etoile::context::Identifier,
		  const elle::String));

//
// ---------- dependencies ----------------------------------------------------
//

#include <agent/Manifest.hh>

#endif
