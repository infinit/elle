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
// updated       julien quintard   [sun mar 21 20:11:45 2010]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/wall/Result.hh>
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
      // Wall
      TagWallIdentify = ::elle::Tags, // XXX
      TagWallChallenge,
      TagWallAuthenticate,
      TagWallAuthenticated,
      TagWallConnect,
      TagWallConnected,

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

// Wall
inward(::etoile::TagWallIdentify,
       parameters(::elle::cryptography::PublicKey));
outward(::etoile::TagWallChallenge,
	parameters(::elle::cryptography::Code));
inward(::etoile::TagWallAuthenticate,
       parameters(::elle::cryptography::Digest));
outward(::etoile::TagWallAuthenticated,
	parameters());

inward(::etoile::TagWallConnect,
       parameters(::elle::core::String));
outward(::etoile::TagWallConnected,
	parameters(::etoile::wall::Result));

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
