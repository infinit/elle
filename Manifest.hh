//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/Manifest.hh
//
// created       julien quintard   [thu mar  4 17:35:00 2010]
// updated       julien quintard   [thu mar 25 20:29:13 2010]
//

#ifndef ELLE_MANIFEST_HH
#define ELLE_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace elle
{

//
// ---------- tags ------------------------------------------------------------
//

  //
  // enumerations
  //
  enum Tag
    {
      TagNone = 0,

      // Error
      TagError,

      Tags = TagError + 1
    };

}

//
// ---------- manifests -------------------------------------------------------
//

///
/// below are the definitions of the messages common to every component
/// relying on the Elle library.
///

// Error
outward(::elle::TagError,
	parameters(const ::elle::misc::Report));

#endif
