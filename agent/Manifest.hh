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
// updated       julien quintard   [sun mar 21 13:20:22 2010]
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
      TagNone = 1000,

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
       parameters(::elle::cryptography::Code));
outward(::agent::TagDecrypted,
        parameters(::elle::cryptography::Clear));

inward(::agent::TagSign,
       parameters(::elle::cryptography::Plain));
outward(::agent::TagSigned,
        parameters(::elle::cryptography::Signature));

#endif
