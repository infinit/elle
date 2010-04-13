//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Manifest.hh
//
// created       julien quintard   [tue apr 13 15:27:49 2010]
// updated       julien quintard   [tue apr 13 15:33:28 2010]
//

#ifndef HOLE_MANIFEST_HH
#define HOLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{

  ///
  /// XXX
  ///
  extern const Character	Component[];

  ///
  /// XXX
  ///
  const Natural32		Tags = 10;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(::hole::Component, ::hole::Tags, ::etoile::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{

  //
  // enumerations
  //
  enum Tag
    {
      TagOk,
      TagBoolean,

      TagPut,
      TagGet,
      TagData,
      TagDestroy
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

outward(::hole::TagOk,
	parameters());
outward(::hole::TagBoolean,
	parameters(const ::elle::core::Boolean));

inward(::hole::TagPut,
       parameters(const ::hole::Address,
		  const ::hole::Data));
inward(::hole::TagGet,
       parameters(const ::hole::Address));
outward(::hole::TagData,
	parameters(const ::hole::Data));
inward(::hole::TagDestroy,
       parameters(const ::hole::Address));

#endif
