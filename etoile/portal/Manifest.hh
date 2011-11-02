//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien.quintard   [mon oct 31 16:02:06 2011]
//

#ifndef ETOILE_PORTAL_MANIFEST_HH
#define ETOILE_PORTAL_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <etoile/path/Chemin.hh>
#include <etoile/path/Slab.hh>
#include <etoile/path/Way.hh>

#include <etoile/gear/Identifier.hh>

#include <etoile/miscellaneous/Abstract.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace etoile
{
  namespace portal
  {

    ///
    /// the component name.
    ///
    extern const elle::Character	Component[];

    ///
    /// this constants defines the number of tags to reserve for
    /// this implementation.
    ///
    const elle::Natural32		Tags = 200;

  }
}

//
// ---------- range -----------------------------------------------------------
//

///
/// this macro-function calls reserves a range of tags.
///
range(etoile::portal::Component,
      etoile::portal::Tags,
      elle::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace etoile
{
  namespace portal
  {

    //
    // enumerations
    //
    enum Tag
      {
	// general
	TagAuthenticate = elle::Range<Component>::First + 1,
	TagAuthenticated,
	TagIdentifier,

	// path
	TagPathResolve,
	TagPathChemin,

	// object
	TagObjectLoad,
	TagObjectInformation,
	TagObjectAbstract,
	TagObjectDiscard,
	TagObjectStore,
	TagObjectDestroy,

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
	TagAccessRevoke,

	// attributes
	TagAttributesSet,
	TagAttributesGet,
	TagAttributesTrait,
	TagAttributesFetch,
	TagAttributesRange,
	TagAttributesOmit
      };

  }
}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the portal messages.
///

// general
message(etoile::portal::TagAuthenticate,
	parameters(lune::Phrase&));
message(etoile::portal::TagAuthenticated,
	parameters());
message(etoile::portal::TagIdentifier,
	parameters(etoile::gear::Identifier&));

// path
message(etoile::portal::TagPathResolve,
	parameters(etoile::path::Way&));
message(etoile::portal::TagPathChemin,
	parameters(etoile::path::Chemin&));

// object
message(etoile::portal::TagObjectLoad,
	parameters(etoile::path::Chemin&));
message(etoile::portal::TagObjectInformation,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagObjectAbstract,
	parameters(etoile::miscellaneous::Abstract&));
message(etoile::portal::TagObjectDiscard,
       parameters(etoile::gear::Identifier&));
message(etoile::portal::TagObjectStore,
       parameters(etoile::gear::Identifier&));
message(etoile::portal::TagObjectDestroy,
       parameters(etoile::gear::Identifier&));

// file
message(etoile::portal::TagFileCreate,
	parameters());
message(etoile::portal::TagFileLoad,
	parameters(etoile::path::Chemin&));
message(etoile::portal::TagFileWrite,
	parameters(etoile::gear::Identifier&,
		   nucleus::Offset&,
		   elle::Region&));
message(etoile::portal::TagFileRead,
	parameters(etoile::gear::Identifier&,
		   nucleus::Offset&,
		   nucleus::Size&));
message(etoile::portal::TagFileRegion,
	parameters(elle::Region&));
message(etoile::portal::TagFileAdjust,
	parameters(etoile::gear::Identifier&,
		   nucleus::Size&));
message(etoile::portal::TagFileDiscard,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagFileStore,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagFileDestroy,
	parameters(etoile::gear::Identifier&));

// directory
message(etoile::portal::TagDirectoryCreate,
	parameters());
message(etoile::portal::TagDirectoryLoad,
	parameters(etoile::path::Chemin&));
message(etoile::portal::TagDirectoryAdd,
	parameters(etoile::gear::Identifier&,
		   etoile::path::Slab&,
		   etoile::gear::Identifier&));
message(etoile::portal::TagDirectoryLookup,
	parameters(etoile::gear::Identifier&,
		   etoile::path::Slab&));
message(etoile::portal::TagDirectoryEntry,
	parameters(nucleus::Entry&));
message(etoile::portal::TagDirectoryConsult,
	parameters(etoile::gear::Identifier&,
		   nucleus::Index&,
		   nucleus::Size&));
message(etoile::portal::TagDirectoryRange,
	parameters(nucleus::Range<nucleus::Entry>&));
message(etoile::portal::TagDirectoryRename,
	parameters(etoile::gear::Identifier&,
		   etoile::path::Slab&,
		   etoile::path::Slab&));
message(etoile::portal::TagDirectoryRemove,
	parameters(etoile::gear::Identifier&,
		   etoile::path::Slab&));
message(etoile::portal::TagDirectoryDiscard,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagDirectoryStore,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagDirectoryDestroy,
	parameters(etoile::gear::Identifier&));

// link
message(etoile::portal::TagLinkCreate,
	parameters());
message(etoile::portal::TagLinkLoad,
	parameters(etoile::path::Chemin&));
message(etoile::portal::TagLinkBind,
	parameters(etoile::gear::Identifier&,
		   etoile::path::Way&));
message(etoile::portal::TagLinkResolve,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagLinkWay,
	parameters(etoile::path::Way&));
message(etoile::portal::TagLinkDiscard,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagLinkStore,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagLinkDestroy,
	parameters(etoile::gear::Identifier&));

// access
message(etoile::portal::TagAccessLookup,
	parameters(etoile::gear::Identifier&,
		   nucleus::Subject&));
message(etoile::portal::TagAccessRecord,
	parameters(nucleus::Record&));
message(etoile::portal::TagAccessConsult,
	parameters(etoile::gear::Identifier&,
		   nucleus::Index&,
		   nucleus::Size&));
message(etoile::portal::TagAccessRange,
	parameters(nucleus::Range<nucleus::Record>&));
message(etoile::portal::TagAccessGrant,
	parameters(etoile::gear::Identifier&,
		   nucleus::Subject&,
		   nucleus::Permissions&));
message(etoile::portal::TagAccessRevoke,
	parameters(etoile::gear::Identifier&,
		   nucleus::Subject&));

// attributes
message(etoile::portal::TagAttributesSet,
	parameters(etoile::gear::Identifier&,
		   elle::String&,
		   elle::String&));
message(etoile::portal::TagAttributesGet,
	parameters(etoile::gear::Identifier&,
		   elle::String&));
message(etoile::portal::TagAttributesTrait,
	parameters(nucleus::Trait&));
message(etoile::portal::TagAttributesFetch,
	parameters(etoile::gear::Identifier&));
message(etoile::portal::TagAttributesRange,
	parameters(nucleus::Range<nucleus::Trait>&));
message(etoile::portal::TagAttributesOmit,
	parameters(etoile::gear::Identifier&,
		   elle::String&));

#endif
