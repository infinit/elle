//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/Plain.hh
//
// created       julien quintard   [wed feb 11 12:39:47 2009]
// updated       julien quintard   [sun may  2 18:41:19 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_PLAIN_HH
#define ELLE_CRYPTOGRAPHY_PLAIN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Region.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this class represents plain data that can be used for cryptographic
    /// purposes.
    ///
    typedef Region	Plain;

  }
}

#endif
