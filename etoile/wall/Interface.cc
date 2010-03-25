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
// updated       julien quintard   [thu mar 25 20:21:26 2010]
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
      /*
      InterfaceRegister(::etoile::TagObjectLoad,
			Object::Load,
			const Path, Identifier);
      InterfaceRegister(::etoile::TagObjectCreate,
			Object::Create,
			const Path, Identifier);
      InterfaceRegister(::etoile::TagObjectStore,
			Object::Store,
			const Identifier);
      InterfaceRegister(::etoile::TagObjectInformation,
			Object::Information,
			const Identifier, Metadata);
      InterfaceRegister(::etoile::TagObjectDestroy,
			Object::Destroy,
			const Identifier);
      */

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
