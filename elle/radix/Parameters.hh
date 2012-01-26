//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun feb 21 19:17:29 2010]
//

#ifndef ELLE_RADIX_PARAMETERS_HH
#define ELLE_RADIX_PARAMETERS_HH

//
// ---------- includes --------------------------------------------------------
//

namespace elle
{
  namespace radix
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// this struct is used to define a set of parameters since it is
    /// not possible to do something like that:
    ///
    ///   typedef T...          Types;
    ///
    /// note that many variadic template classes then rely on the
    /// Parameters structure such as Callback, Arguments, Closure etc.
    ///
    /// this structure is especially useful because a Callback, for instance,
    /// since parameterized by a Paramters<T...>, can retrieve the consituent
    /// types of the Parameters.
    ///
    template <typename... T>
    struct Parameters
    {
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Parameters.hxx>

#endif
