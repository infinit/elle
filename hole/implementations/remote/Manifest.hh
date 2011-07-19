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
// updated       julien quintard   [mon jul 18 20:15:29 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_MANIFEST_HH

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
    namespace remote
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
	  TagOk = elle::Range<Component>::First,

	  TagChallenge,
	  TagResponse,

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

outward(hole::implementations::remote::TagOk,
	parameters());

outward(hole::implementations::remote::TagChallenge,
	parameters())
inward(hole::implementations::remote::TagResponse,
       parameters(elle::Cipher));

inward(hole::implementations::remote::TagPush,
       parameters(nucleus::Address,
		  nucleus::Derivable<nucleus::Block>));
inward(hole::implementations::remote::TagPull,
       parameters(nucleus::Address,
		  nucleus::Version));
outward(hole::implementations::remote::TagBlock,
	parameters(nucleus::Derivable<nucleus::Block>));
inward(hole::implementations::remote::TagWipe,
       parameters(nucleus::Address));

#endif
