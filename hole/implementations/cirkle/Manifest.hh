//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/cirkle/Manifest.hh
//
// created       julien quintard   [fri aug 12 15:18:12 2011]
// updated       julien quintard   [fri aug 12 15:56:18 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_MANIFEST_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace cirkle
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
  }
}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(hole::implementations::cirkle::Component,
      hole::implementations::cirkle::Tags,
      elle::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

      //
      // enumerations
      //
      enum Tag
	{
	  TagPropagate = elle::Range<Component>::First + 1,

	  TagPush,
	  TagPull,
	  TagBlock,
	  TagWipe
	};

    }
  }
}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

message(hole::implementations::cirkle::TagPropagate,
	parameters(hole::implementations::cirkle::RoutingTable))

inward(hole::implementations::cirkle::TagPush,
       parameters(nucleus::Address,
		  nucleus::Derivable<nucleus::Block>));
inward(hole::implementations::cirkle::TagPull,
       parameters(nucleus::Address,
		  nucleus::Version));
outward(hole::implementations::cirkle::TagBlock,
	parameters(nucleus::Derivable<nucleus::Block>));
inward(hole::implementations::cirkle::TagWipe,
       parameters(nucleus::Address));

#endif
