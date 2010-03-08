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
// updated       julien quintard   [thu mar  4 18:32:46 2010]
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
      TagUserAuthenticate,
      TagUserConnect,

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
Inward(::etoile::TagUserIdentify,
       Parameters(::elle::crypto::PublicKey,
		  ::elle::core::String));
Outward(::etoile::TagUserIdentify,
	Parameters(::elle::crypto::Code));

Inward(::etoile::TagUserAuthenticate,
       Parameters(::elle::crypto::Digest));
Outward(::etoile::TagUserAuthenticate,
	Parameters(::etoile::wall::Report));

Inward(::etoile::TagUserConnect,
       Parameters(::elle::core::String));
Outward(::etoile::TagUserConnect,
	Parameters(::etoile::wall::Report));

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
