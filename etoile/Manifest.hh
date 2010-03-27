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
// updated       julien quintard   [sat mar 27 06:43:52 2010]
//

#ifndef ETOILE_MANIFEST_HH
#define ETOILE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/wall/Types.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace etoile
{

  ///
  /// XXX
  ///
  extern const Character	Component[];

  ///
  /// XXX
  ///
  const Natural32		Tags = 100;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(::etoile::Component, ::etoile::Tags, ::elle::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace etoile
{

  //
  // enumerations
  //
  enum Tag
    {
      // Wall
      TagWallIdentify = Range<Component>::First,
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
      TagObjectAccessUpdate
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

//
// ---------- dependencies ----------------------------------------------------
//

#include <agent/Manifest.hh>

#endif
