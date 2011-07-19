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
// updated       julien quintard   [tue jul 19 16:08:36 2011]
//

#ifndef ELLE_RADIX_ARGUMENTS_HH
#define ELLE_RADIX_ARGUMENTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Parameters.hh>

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

    ///
    /// this specialized class is meaningless but is used to hold
    /// the Arguments-related functionalities together.
    ///
    template <>
    class Arguments<>
    {
    public:
      //
      // methods
      //
      template <template <typename...> class E1,
		template <typename...> class E2,
		typename... U,
		typename... V>
      static
      Arguments< Parameters<U..., V...> >
      Union(E1< Parameters<U...> >&,
	    E2< Parameters<V...> >&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/radix/Arguments.hxx>

#endif
