//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/core/Type.hh
//
// created       julien quintard   [mon apr 26 15:49:09 2010]
// updated       julien quintard   [mon apr 26 15:54:57 2010]
//

#ifndef ELLE_CORE_TYPE_HH
#define ELLE_CORE_TYPE_HH

namespace elle
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides two methods for enabling the programmer to
    /// retrieve capacity properties according to a variable instead
    /// of a type.
    ///
    class Variable
    {
    public:
      //
      // methods
      //
      template <typename T>
      static T		Minimum(const T&);

      template <typename T>
      static T		Maximum(const T&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/core/Type.hxx>

#endif
