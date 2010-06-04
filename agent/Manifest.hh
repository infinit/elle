//
// ---------- header ----------------------------------------------------------
//
// project       agent
//
// license       infinit
//
// file          /home/mycure/infinit/agent/Manifest.hh
//
// created       julien quintard   [thu mar  4 17:34:08 2010]
// updated       julien quintard   [mon may 24 22:23:21 2010]
//

#ifndef AGENT_MANIFEST_HH
#define AGENT_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/Manifest.hh>

//
// ---------- constants -------------------------------------------------------
//

namespace agent
{

  ///
  /// XXX
  ///
  extern const elle::Character	Component[];

  ///
  /// XXX
  ///
  const elle::Natural32		Tags = 2;

}

//
// ---------- range -----------------------------------------------------------
//

///
/// XXX
///
range(agent::Component, agent::Tags, etoile::Component);

//
// ---------- tags ------------------------------------------------------------
//

namespace agent
{

  //
  // enumerations
  //
  enum Tag
    {
      TagDecrypt = elle::Range<Component>::First,
      TagDecrypted,
      TagSign,
      TagSigned,
    };

}

//
// ---------- manifest --------------------------------------------------------
//

///
/// below are the definitions of the Agent messages.
///

inward(agent::TagDecrypt,
       parameters(const elle::String,
		  const elle::Code));
outward(agent::TagDecrypted,
        parameters(const elle::Clear));

inward(agent::TagSign,
       parameters(const elle::String,
		  const elle::Plain));
outward(agent::TagSigned,
        parameters(const elle::Signature));

#endif
