#ifndef ELLE_RADIX_SET_HH
#define ELLE_RADIX_SET_HH

// XXX[class to delete]

#include <elle/radix/Parameters.hh>

namespace elle
{
  namespace radix
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// the set structure provides templates for manipulating Parameters
    /// types in order to perform set-theory-like operations.
    ///
    struct Set
    {
      //
      // structures
      //
      template <typename... T>
      struct Union
      {
      };
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Set.hxx>

#endif
