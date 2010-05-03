//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Interface.cc
//
// created       julien quintard   [wed mar  3 18:30:05 2010]
// updated       julien quintard   [mon may  3 18:54:58 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Interface.hh>
#include <etoile/wall/Wall.hh>
#include <etoile/wall/Object.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this array contains the interface callbacks.
    ///
    elle::Routine*		Interface::Callbacks[etoile::Tags];

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the interface by registering the
    /// callbacks for inward messages.
    ///
    elle::Status	Interface::Initialize()
    {
      enter();

      // initialize the structure to zero.
      ::memset(Interface::Callbacks, 0x0, sizeof(Interface::Callbacks));

      // user
      InterfaceRegister(etoile::TagWallIdentify,
			Wall::Identify,
			parameters(const elle::PublicKey));
      InterfaceRegister(etoile::TagWallAuthenticate,
			Wall::Authenticate,
			parameters(const elle::Digest));
      InterfaceRegister(etoile::TagWallConnect,
			Wall::Connect,
			parameters(const elle::String));

      // object
      InterfaceRegister(etoile::TagObjectLoad,
			Object::Load,
			parameters(const path::Way));
      InterfaceRegister(etoile::TagObjectInformation,
			Object::Information,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagObjectDiscard,
			Object::Discard,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagObjectStore,
			Object::Store,
			parameters(const context::Identifier));

      // directory
      InterfaceRegister(etoile::TagDirectoryCreate,
			Directory::Create,
			parameters());
      InterfaceRegister(etoile::TagDirectoryLoad,
			Directory::Load,
			parameters(const path::Way));
      InterfaceRegister(etoile::TagDirectoryAdd,
			Directory::Add,
			parameters(const context::Identifier,
				   const path::Slice,
				   const context::Identifier));
      InterfaceRegister(etoile::TagDirectoryLookup,
			Directory::Lookup,
			parameters(const context::Identifier,
				   const path::Slice));
      InterfaceRegister(etoile::TagDirectoryConsult,
			Directory::Consult,
			parameters(const context::Identifier,
				   const kernel::Index,
				   const kernel::Size));
      InterfaceRegister(etoile::TagDirectoryRename,
			Directory::Rename,
			parameters(const context::Identifier,
				   const path::Slice,
				   const path::Slice));
      InterfaceRegister(etoile::TagDirectoryRemove,
			Directory::Remove,
			parameters(const context::Identifier,
				   const path::Slice));
      InterfaceRegister(etoile::TagDirectoryDiscard,
			Directory::Discard,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagDirectoryStore,
			Directory::Store,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagDirectoryDestroy,
			Directory::Destroy,
			parameters(const context::Identifier));

      // file
      InterfaceRegister(etoile::TagFileCreate,
			File::Create,
			parameters());
      InterfaceRegister(etoile::TagFileLoad,
			File::Load,
			parameters(const path::Way));
      InterfaceRegister(etoile::TagFileWrite,
			File::Write,
			parameters(const context::Identifier,
				   const kernel::Offset,
				   const elle::Region));
      InterfaceRegister(etoile::TagFileRead,
			File::Read,
			parameters(const context::Identifier,
				   const kernel::Offset,
				   const kernel::Size));
      InterfaceRegister(etoile::TagFileAdjust,
			File::Adjust,
			parameters(const context::Identifier,
				   const kernel::Size));
      InterfaceRegister(etoile::TagFileDiscard,
			File::Discard,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagFileStore,
			File::Store,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagFileDestroy,
			File::Destroy,
			parameters(const context::Identifier));

      // link
      InterfaceRegister(etoile::TagLinkCreate,
			Link::Create,
			parameters());
      InterfaceRegister(etoile::TagLinkLoad,
			Link::Load,
			parameters(const path::Way));
      InterfaceRegister(etoile::TagLinkBind,
			Link::Bind,
			parameters(const context::Identifier,
				   const path::Way));
      InterfaceRegister(etoile::TagLinkResolve,
			Link::Resolve,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagLinkDiscard,
			Link::Discard,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagLinkStore,
			Link::Store,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagLinkDestroy,
			Link::Destroy,
			parameters(const context::Identifier));

      // access
      InterfaceRegister(etoile::TagAccessLookup,
			Access::Lookup,
			parameters(const context::Identifier,
				   const kernel::Subject));
      InterfaceRegister(etoile::TagAccessConsult,
			Access::Consult,
			parameters(const context::Identifier,
				   const kernel::Index,
				   const kernel::Size));
      InterfaceRegister(etoile::TagAccessGrant,
			Access::Grant,
			parameters(const context::Identifier,
				   const kernel::Subject,
				   const kernel::Permissions));
      InterfaceRegister(etoile::TagAccessUpdate,
			Access::Update,
			parameters(const context::Identifier,
				   const kernel::Subject,
				   const kernel::Permissions));
      InterfaceRegister(etoile::TagAccessBlock,
			Access::Block,
			parameters(const context::Identifier,
				   const kernel::Subject));
      InterfaceRegister(etoile::TagAccessRevoke,
			Access::Revoke,
			parameters(const context::Identifier,
				   const kernel::Subject));

      // attributes
      InterfaceRegister(etoile::TagAttributesSet,
			Attributes::Set,
			parameters(const context::Identifier,
				   const elle::String,
				   const elle::String));
      InterfaceRegister(etoile::TagAttributesGet,
			Attributes::Get,
			parameters(const context::Identifier,
				   const elle::String));
      InterfaceRegister(etoile::TagAttributesFetch,
			Attributes::Fetch,
			parameters(const context::Identifier));
      InterfaceRegister(etoile::TagAttributesOmit,
			Attributes::Omit,
			parameters(const context::Identifier,
				   const elle::String));

      leave();
    }

    ///
    /// this method cleans the interface by deleting the callbacks.
    ///
    elle::Status	Interface::Clean()
    {
      elle::Natural32	i;

      enter();

      // delete the callbacks.
      for (i = elle::TagNone; i < etoile::Tags; i++)
	{
	  if (Interface::Callbacks[i] != NULL)
	    delete Interface::Callbacks[i];
	}

      leave();
    }

  }
}
