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
// updated       julien quintard   [thu mar 25 21:21:44 2010]
//

#ifndef AGENT_MANIFEST_HH
#define AGENT_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
      /// XXX \todo trouver un truc plus propre pour eviter les collisions
      TagNone = 10000,

      TagDecrypt,
      TagDecrypted,
      TagSign,
      TagSigned,

      Tags = TagSign + 1
    };

}

//
// ---------- manifest --------------------------------------------------------
//

inward(::agent::TagDecrypt,
       parameters(const ::elle::cryptography::Code));
outward(::agent::TagDecrypted,
        parameters(const ::elle::cryptography::Clear));

inward(::agent::TagSign,
       parameters(const ::elle::cryptography::Plain));
outward(::agent::TagSigned,
        parameters(const ::elle::cryptography::Signature));

#endif
