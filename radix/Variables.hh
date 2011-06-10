//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Variables.hh
//
// created       julien quintard   [wed feb 24 08:03:32 2010]
// updated       julien quintard   [thu jun  9 19:01:50 2011]
//

#ifndef ELLE_RADIX_VARIABLES_HH
#define ELLE_RADIX_VARIABLES_HH

namespace elle
{
  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class wraps a set of variables which are about to be
    /// initialized.
    ///
    /// the class provides a method, i.e Trigger(), for triggering a
    /// callback/closure/etc. with the variables.
    ///
    /// note that the Trigger() method is provided both for non-const
    /// and const environments.
    ///
    template <typename... T>
    class Variables
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Variables.hxx>

#endif
