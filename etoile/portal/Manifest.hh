#ifndef ETOILE_PORTAL_MANIFEST_HH
# define ETOILE_PORTAL_MANIFEST_HH

# include <elle/types.hh>
# include <elle/Manifest.hh>
# include <elle/standalone/fwd.hh>
# include <elle/network/Range.hh>
# include <etoile/path/fwd.hh>
# include <etoile/gear/fwd.hh>
# include <etoile/miscellaneous/fwd.hh>
# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <elle/idiom/Open.hh>

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
    extern const elle::Character        Component[];

    ///
    /// this constants defines the number of tags to reserve for
    /// this implementation.
    ///
    const elle::Natural32               Tags = 200;

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
        TagAuthenticate = elle::network::Range<Component>::First + 1,
        TagAuthenticated,
        TagIdentifier,

        // path
        TagPathResolve,
        TagPathChemin,
        TagPathLocate,
        TagPathWay,

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
        TagAttributesOmit,

        // group
        TagGroupCreate,
        TagGroupIdentity,
        TagGroupLoad,
        TagGroupAdd,
        TagGroupLookup,
        TagGroupFellow,
        TagGroupConsult,
        TagGroupRange,
        TagGroupRemove,
        TagGroupDiscard,
        TagGroupStore,
        TagGroupDestroy
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
        parameters(elle::String&));
message(etoile::portal::TagAuthenticated,
        parameters());
message(etoile::portal::TagIdentifier,
        parameters(etoile::gear::Identifier&));

// path
message(etoile::portal::TagPathResolve,
        parameters(etoile::path::Way&));
message(etoile::portal::TagPathChemin,
        parameters(etoile::path::Chemin&));
message(etoile::portal::TagPathLocate,
        parameters(etoile::path::Way&));
message(etoile::portal::TagPathWay,
        parameters(etoile::path::Way&));

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
                   nucleus::neutron::Offset&,
                   elle::standalone::Region&));
message(etoile::portal::TagFileRead,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Offset&,
                   nucleus::neutron::Size&));
message(etoile::portal::TagFileRegion,
        parameters(elle::standalone::Region&));
message(etoile::portal::TagFileAdjust,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Size&));
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
        parameters(nucleus::neutron::Entry&));
message(etoile::portal::TagDirectoryConsult,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Index&,
                   nucleus::neutron::Size&));
message(etoile::portal::TagDirectoryRange,
        parameters(nucleus::neutron::Range<
                     nucleus::neutron::Entry>&));
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
                   nucleus::neutron::Subject&));
message(etoile::portal::TagAccessRecord,
        parameters(nucleus::neutron::Record&));
message(etoile::portal::TagAccessConsult,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Index&,
                   nucleus::neutron::Size&));
message(etoile::portal::TagAccessRange,
        parameters(nucleus::neutron::Range<
                     nucleus::neutron::Record>&));
message(etoile::portal::TagAccessGrant,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Subject&,
                   nucleus::neutron::Permissions&));
message(etoile::portal::TagAccessRevoke,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Subject&));

// attributes
message(etoile::portal::TagAttributesSet,
        parameters(etoile::gear::Identifier&,
                   elle::String&,
                   elle::String&));
message(etoile::portal::TagAttributesGet,
        parameters(etoile::gear::Identifier&,
                   elle::String&));
message(etoile::portal::TagAttributesTrait,
        parameters(nucleus::neutron::Trait&));
message(etoile::portal::TagAttributesFetch,
        parameters(etoile::gear::Identifier&));
message(etoile::portal::TagAttributesRange,
        parameters(nucleus::neutron::Range<
                     nucleus::neutron::Trait>&));
message(etoile::portal::TagAttributesOmit,
        parameters(etoile::gear::Identifier&,
                   elle::String&));

// group
message(etoile::portal::TagGroupCreate,
        parameters(elle::String&));
message(etoile::portal::TagGroupIdentity,
        parameters(typename nucleus::neutron::Group::Identity&,
                   etoile::gear::Identifier&));
message(etoile::portal::TagGroupLoad,
        parameters(typename nucleus::neutron::Group::Identity&));
message(etoile::portal::TagGroupAdd,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Subject&));
message(etoile::portal::TagGroupLookup,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Subject&));
message(etoile::portal::TagGroupFellow,
        parameters(nucleus::neutron::Fellow&));
message(etoile::portal::TagGroupConsult,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Index&,
                   nucleus::neutron::Size&));
message(etoile::portal::TagGroupRange,
        parameters(nucleus::neutron::Range<nucleus::neutron::Fellow>&));
message(etoile::portal::TagGroupRemove,
        parameters(etoile::gear::Identifier&,
                   nucleus::neutron::Subject&));
message(etoile::portal::TagGroupDiscard,
        parameters(etoile::gear::Identifier&));
message(etoile::portal::TagGroupStore,
        parameters(etoile::gear::Identifier&));
message(etoile::portal::TagGroupDestroy,
        parameters(etoile::gear::Identifier&));

#endif
