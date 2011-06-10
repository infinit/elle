//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Arguments.hh
//
// created       julien quintard   [wed feb 24 08:03:32 2010]
// updated       julien quintard   [thu jun  9 23:44:43 2011]
//

#ifndef ELLE_RADIX_ARGUMENTS_HH
#define ELLE_RADIX_ARGUMENTS_HH

namespace elle
{
  namespace radix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class wraps a set of arguments and provides a method, i.e
    /// Trigger(), for triggering a callback/closure/etc. with the recorded
    /// set of arguments.
    ///
    /// note that the Trigger() method is provided both for non-const
    /// and const environments.
    ///
    /// this construct turns out to be extremely useful in a large number
    /// of situations involving an undefined number of parameters.
    ///
    template <typename... T>
    class Arguments
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Arguments.hxx>

#endif
