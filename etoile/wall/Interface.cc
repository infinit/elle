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
// updated       julien quintard   [fri apr 16 11:53:02 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Interface.hh>

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
    Routine*		Interface::Callbacks[::etoile::Tags];

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the interface by registering the
    /// callbacks for inward messages.
    ///
    Status		Interface::Initialize()
    {
      enter();

      // user
      InterfaceRegister(::etoile::TagWallIdentify,
			Wall::Identify,
			parameters(const PublicKey));
      InterfaceRegister(::etoile::TagWallAuthenticate,
			Wall::Authenticate,
			parameters(const Digest));
      InterfaceRegister(::etoile::TagWallConnect,
			Wall::Connect,
			parameters(const String));

      // object
      InterfaceRegister(::etoile::TagObjectLoad,
			Object::Load,
			parameters(const path::Way));
      InterfaceRegister(::etoile::TagObjectInformation,
			Object::Information,
			parameters(const context::Identifier));
      InterfaceRegister(::etoile::TagObjectStore,
			Object::Store,
			parameters(const context::Identifier));

      // directory
      InterfaceRegister(::etoile::TagDirectoryLoad,
			Directory::Load,
			parameters(const path::Way));
      InterfaceRegister(::etoile::TagDirectoryCreate,
			Directory::Create,
			parameters());
      InterfaceRegister(::etoile::TagDirectoryAdd,
			Directory::Add,
			parameters(const context::Identifier,
				   const path::Slice,
				   const context::Identifier));
      InterfaceRegister(::etoile::TagDirectoryLookup,
			Directory::Lookup,
			parameters(const context::Identifier,
				   const path::Slice));
      InterfaceRegister(::etoile::TagDirectoryConsult,
			Directory::Consult,
			parameters(const context::Identifier,
				   const kernel::Index,
				   const kernel::Size));
      InterfaceRegister(::etoile::TagDirectoryRename,
			Directory::Rename,
			parameters(const context::Identifier,
				   const path::Slice,
				   const path::Slice));
      InterfaceRegister(::etoile::TagDirectoryRemove,
			Directory::Remove,
			parameters(const context::Identifier,
				   const path::Slice));
      InterfaceRegister(::etoile::TagDirectoryStore,
			Directory::Store,
			parameters(const context::Identifier));
      InterfaceRegister(::etoile::TagDirectoryDestroy,
			Directory::Destroy,
			parameters(const context::Identifier));

      // access
      InterfaceRegister(::etoile::TagAccessLookup,
			Access::Lookup,
			parameters(const context::Identifier,
				   const kernel::Subject));
      InterfaceRegister(::etoile::TagAccessConsult,
			Access::Consult,
			parameters(const context::Identifier,
				   const kernel::Index,
				   const kernel::Size));
      InterfaceRegister(::etoile::TagAccessGrant,
			Access::Grant,
			parameters(const context::Identifier,
				   const kernel::Subject,
				   const kernel::Permissions));
      InterfaceRegister(::etoile::TagAccessUpdate,
			Access::Update,
			parameters(const context::Identifier,
				   const kernel::Subject,
				   const kernel::Permissions));
      InterfaceRegister(::etoile::TagAccessBlock,
			Access::Block,
			parameters(const context::Identifier,
				   const kernel::Subject));
      InterfaceRegister(::etoile::TagAccessRevoke,
			Access::Revoke,
			parameters(const context::Identifier,
				   const kernel::Subject));

      // attributes
      InterfaceRegister(::etoile::TagAttributesAdd,
			Attributes::Add,
			parameters(const context::Identifier,
				   const String,
				   const String));
      InterfaceRegister(::etoile::TagAttributesLookup,
			Attributes::Lookup,
			parameters(const context::Identifier,
				   const String));
      InterfaceRegister(::etoile::TagAttributesConsult,
			Attributes::Consult,
			parameters(const context::Identifier,
				   const kernel::Index,
				   const kernel::Size));
      InterfaceRegister(::etoile::TagAttributesUpdate,
			Attributes::Update,
			parameters(const context::Identifier,
				   const String,
				   const String));
      InterfaceRegister(::etoile::TagAttributesRemove,
			Attributes::Remove,
			parameters(const context::Identifier,
				   const String));

      leave();
    }

    ///
    /// this method cleans the interface by deleting the callbacks.
    ///
    Status		Interface::Clean()
    {
      Natural32		i;

      enter();

      // delete the callbacks.
      for (i = TagNone; i < ::etoile::Tags; i++)
	delete Interface::Callbacks[i];

      leave();
    }

  }
}
