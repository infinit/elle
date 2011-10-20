//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 12 15:18:12 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH
#define HOLE_IMPLEMENTATIONS_SLUG_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <elle/Manifest.hh>

#include <lune/Lune.hh>

#include <hole/implementations/remote/Manifest.hh>
#include <hole/implementations/slug/Cluster.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace slug
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
range(hole::implementations::slug::Component,
      hole::implementations::slug::Tags,
      hole::implementations::remote::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      //
      // enumerations
      //
      enum Tag
	{
	  TagChallenge = elle::Range<Component>::First + 1,
	  TagPassport,
	  TagPort,
	  TagAuthenticated,

	  TagUpdate,

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

message(hole::implementations::slug::TagChallenge,
	parameters())
message(hole::implementations::slug::TagPassport,
	parameters(lune::Passport))
message(hole::implementations::slug::TagPort,
	parameters(elle::Port))
message(hole::implementations::slug::TagAuthenticated,
	parameters())

message(hole::implementations::slug::TagUpdate,
	parameters(hole::implementations::slug::Cluster))

message(hole::implementations::slug::TagPush,
	parameters(nucleus::Address,
		   nucleus::Derivable<nucleus::Block>));
message(hole::implementations::slug::TagPull,
	parameters(nucleus::Address,
		   nucleus::Version));
message(hole::implementations::slug::TagBlock,
	parameters(nucleus::Derivable<nucleus::Block>));
message(hole::implementations::slug::TagWipe,
	parameters(nucleus::Address));

#endif
