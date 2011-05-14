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
// updated       julien quintard   [fri may 13 14:48:44 2011]
//

#ifndef HOLE_MANIFEST_HH
#define HOLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <agent/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{

  ///
  /// XXX
  ///
  extern const elle::Character	Component[];

  ///
  /// XXX
  ///
  const elle::Natural32		Tags = 10;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(hole::Component, hole::Tags, agent::Component);

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
      TagOk = elle::Range<Component>::First,
      TagBoolean,

      TagJoin,
      TagLeave,
      TagPut,
      TagGet,
      TagBlock,
      TagErase
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

outward(hole::TagOk,
	parameters());
outward(hole::TagBoolean,
	parameters(const elle::core::Boolean));

inward(hole::TagJoin,
       parameters(const nucleus::Network));
inward(hole::TagLeave,
       parameters(const nucleus::Network));

inward(hole::TagPut,
       parameters(const nucleus::Network,
		  const nucleus::Address,
		  const elle::Derivable<nucleus::Block>));
inward(hole::TagGet,
       parameters(const nucleus::Network,
		  const nucleus::Address));
outward(hole::TagBlock,
	parameters(const elle::Derivable<nucleus::Block>));
inward(hole::TagErase,
       parameters(const nucleus::Network,
		  const nucleus::Address));

#endif
