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
// updated       julien quintard   [thu mar 25 20:21:00 2010]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
       parameters(const ::elle::cryptography::PublicKey));
outward(::etoile::TagWallChallenge,
	parameters(const ::elle::cryptography::Code));
inward(::etoile::TagWallAuthenticate,
       parameters(const ::elle::cryptography::Digest));
outward(::etoile::TagWallAuthenticated,
	parameters());

inward(::etoile::TagWallConnect,
       parameters(const ::elle::core::String));
outward(::etoile::TagWallConnected,
	parameters());

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
