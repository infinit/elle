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
// updated       julien quintard   [thu mar  4 17:56:36 2010]
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
    Callback*		Interface::Callbacks[::etoile::Tags];

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
      InterfaceRegister(::etoile::TagUserIdentify,
			User::Identify,
			Parameters(const PublicKey,
				   const String));
      InterfaceRegister(::etoile::TagUserAuthenticate,
			User::Authenticate,
			Parameters(const Digest));
      InterfaceRegister(::etoile::TagUserConnect,
			User::Connect,
			Parameters(const String));

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
      for (i = ::etoile::TagNone; i < ::etoile::Tags; i++)
	delete Interface::Callbacks[i];

      leave();
    }

  }
}
