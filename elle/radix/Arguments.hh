#ifndef ELLE_RADIX_ARGUMENTS_HH
#define ELLE_RADIX_ARGUMENTS_HH

#include <elle/radix/Parameters.hh>

// XXX[class to delete]

namespace elle
{
  namespace radix
  {

    ///
    /// this class wraps a set of arguments and provides methods, i.e
    /// Call() and Trigger(), for triggering a callback/closure/etc. with
    /// the recorded set of arguments.
    ///
    /// note that the methods are provided both for non-const and const
    /// environments.
    ///
    /// this construct turns out to be extremely useful in a large number
    /// of situations involving an undefined number of parameters.
    ///
    /// note that constructors are also provided for constructing an
    /// Arguments instances based on a Variables or else.
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
