#ifndef ELLE_CRYPTOGRAPHY_CLEAR_HH
# define ELLE_CRYPTOGRAPHY_CLEAR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Plain.hh>

namespace elle
{
  namespace cryptography
  {

    ///
    /// this class represents a decoded text, both through symmetric and
    /// asymmetric cryptosystems.
    ///
    /// this type is logically equivalent to a plain text.
    ///
    typedef Plain       Clear;

  }
}

#endif
#include <elle/cryptography/Clear.hxx>
