//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Manifest.hh
//
// created       julien quintard   [thu may 26 12:59:43 2011]
// updated       julien quintard   [thu may 26 13:16:06 2011]
//

#ifndef HOLE_REMOTE_MANIFEST_HH
#define HOLE_REMOTE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace hole
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

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(hole::remote::Component, hole::remote::Tags, hole::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace hole
{
  namespace remote
  {

    //
    // enumerations
    //
    enum Tag
      {
	TagRemoteOk = elle::Range<Component>::First,

	TagRemotePush,
	TagRemotePull,
	TagRemoteBlock,
	TagRemoteWipe
      };

  }
}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the inward and outward messages.
///

outward(hole::remote::TagRemoteOk,
	parameters());

inward(hole::remote::TagRemotePush,
       parameters(const nucleus::Network,
		  const nucleus::Address,
		  const elle::Derivable<nucleus::Block>));
inward(hole::remote::TagRemotePull,
       parameters(const nucleus::Network,
		  const nucleus::Address,
		  const nucleus::Version));
outward(hole::remote::TagRemoteBlock,
	parameters(const elle::Derivable<nucleus::Block>));
inward(hole::remote::TagRemoteWipe,
       parameters(const nucleus::Network,
		  const nucleus::Address));

#endif
