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
// updated       julien quintard   [sun apr 18 20:03:39 2010]
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
  extern const Character	Component[];

  ///
  /// XXX
  ///
  const Natural32		Tags = 2;

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
      TagDecrypt = Range<Component>::First,
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
       parameters(const elle::cryptography::Code));
outward(agent::TagDecrypted,
        parameters(const elle::cryptography::Clear));

inward(agent::TagSign,
       parameters(const elle::cryptography::Plain));
outward(agent::TagSigned,
        parameters(const elle::cryptography::Signature));

#endif
