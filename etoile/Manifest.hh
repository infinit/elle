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
// updated       julien quintard   [wed mar 10 22:09:05 2010]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

//#include <etoile/wall/Wall.hh>
#include <etoile/wall/Types.hh>

namespace etoile
{

//
// ---------- tags ------------------------------------------------------------
//

  //
  // enumerations
  //
  enum Tag
    {
      /// XXX \todo trouver un truc plus propre pour eviter les collisions
      TagNone = 0,

      // User
      TagUserIdentify,
      TagUserChallenge,
      TagUserAuthenticate,
      TagUserAuthenticated,
      TagUserConnect,
      TagUserConnected,

      // Object
      TagObjectLoad,
      TagObjectCreate,
      TagObjectStore,
      TagObjectInformation,
      TagObjectDestroy,
      TagObjectAccessList,
      TagObjectAccessGrant,
      TagObjectAccessRevoke,
      TagObjectAccessUpdate,

      Tags = TagObjectAccessUpdate + 1
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

// User

inward(::etoile::TagUserIdentify,
       parameters(::elle::cryptography::PublicKey,
		  ::elle::core::String));
outward(::etoile::TagUserChallenge,
	parameters(::elle::cryptography::Code));
inward(::etoile::TagUserAuthenticate,
       parameters(::elle::cryptography::Digest));
outward(::etoile::TagUserAuthenticated,
	parameters(::etoile::wall::Report));

inward(::etoile::TagUserConnect,
       parameters(::elle::core::String));
outward(::etoile::TagUserConnected,
	parameters(::etoile::wall::Report));

// Object
/*
Conversation(::etoile::TagObjectLoad,
	 ::etoile::wall::Path,
	 ::etoile::wall::Identifier);
Manifest(::etoile::TagObjectCreate,
	 ::etoile::wall::Path,
	 ::etoile::wall::Identifier);
Manifest(::etoile::TagObjectStore,
	 ::etoile::wall::Identifier);
Manifest(::etoile::TagObjectInformation,
	 ::etoile::wall::Identifier);
Manifest(::etoile::TagObjectDestroy,
	 ::etoile::wall::Identifier);
*/

#endif
