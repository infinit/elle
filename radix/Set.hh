//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Set.hh
//
// created       julien quintard   [fri jul 15 14:49:26 2011]
// updated       julien quintard   [sat sep  3 20:56:52 2011]
//

#ifndef ELLE_RADIX_SET_HH
#define ELLE_RADIX_SET_HH

//
// ---------- includes --------------------------------------------------------
//

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
