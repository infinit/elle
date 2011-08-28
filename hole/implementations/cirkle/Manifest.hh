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
// updated       julien quintard   [sun aug 28 20:53:06 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_MANIFEST_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <elle/Manifest.hh>

#include <lune/Lune.hh>

#include <hole/implementations/remote/Manifest.hh>
#include <hole/implementations/cirkle/Cluster.hh>

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
      const elle::Natural32		Tags = 30;

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
      hole::implementations::remote::Component);

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
	  TagChallenge = elle::Range<Component>::First + 1,
	  TagAuthenticate,
	  TagAuthenticated,

	  TagListen,
	  TagPort,

	  TagRequest,
	  TagGossip,

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

message(hole::implementations::cirkle::TagChallenge,
	parameters())
message(hole::implementations::cirkle::TagAuthenticate,
	parameters(lune::Passport))
message(hole::implementations::cirkle::TagAuthenticated,
	parameters())

message(hole::implementations::cirkle::TagListen,
	parameters())
message(hole::implementations::cirkle::TagPort,
	parameters(elle::Port))

message(hole::implementations::cirkle::TagRequest,
	parameters())
message(hole::implementations::cirkle::TagGossip,
	parameters(hole::implementations::cirkle::Cluster))

message(hole::implementations::cirkle::TagPush,
	parameters(nucleus::Address,
		   nucleus::Derivable<nucleus::Block>));
message(hole::implementations::cirkle::TagPull,
	parameters(nucleus::Address,
		   nucleus::Version));
message(hole::implementations::cirkle::TagBlock,
	parameters(nucleus::Derivable<nucleus::Block>));
message(hole::implementations::cirkle::TagWipe,
	parameters(nucleus::Address));

#endif
