//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/Clear.hh
//
// created       julien quintard   [tue oct 30 10:05:43 2007]
// updated       julien quintard   [sat mar  7 23:02:16 2009]
//

#ifndef ELLE_CRYPTOGRAPHY_CLEAR_HH
#define ELLE_CRYPTOGRAPHY_CLEAR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Plain.hh>

namespace elle
{
  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a decoded text, both through symmetric and
    /// asymmetric cryptosystems.
    ///
    /// this type is logically equivalent to a plain text.
    ///
    typedef Plain	Clear;

  }
}

#endif
