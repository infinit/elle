//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Manifest.hh
//
// created       julien quintard   [thu may 26 12:59:43 2011]
// updated       julien quintard   [wed aug 31 21:58:57 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <elle/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      ///
      /// XXX
      ///
      extern const elle::Character	Component[];

      ///
      /// XXX
      ///
      const elle::Natural32		Tags = 20;

    }
  }
}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(hole::implementations::remote::Component,
      hole::implementations::remote::Tags,
      elle::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      //
      // enumerations
      //
      enum Tag
	{
	  TagChallenge = elle::Range<Component>::First + 1,
	  TagResponse,
	  TagAuthenticated,

	  TagPush,
	  TagPull,
	  TagBlock,
	  TagWipe,

	  TagError
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

message(hole::implementations::remote::TagChallenge,
	parameters())
message(hole::implementations::remote::TagResponse,
	parameters(lune::Passport));
message(hole::implementations::remote::TagAuthenticated,
	parameters());

message(hole::implementations::remote::TagPush,
	parameters(nucleus::Address,
		   nucleus::Derivable<nucleus::Block>));
message(hole::implementations::remote::TagPull,
	parameters(nucleus::Address,
		   nucleus::Version));
message(hole::implementations::remote::TagBlock,
	parameters(nucleus::Derivable<nucleus::Block>));
message(hole::implementations::remote::TagWipe,
	parameters(nucleus::Address));

message(hole::implementations::remote::TagError,
	parameters(elle::Report));

#endif
