#ifndef ELLE_RADIX_VARIABLES_HH
#define ELLE_RADIX_VARIABLES_HH

// XXX[class to delete]

#include <elle/radix/Parameters.hh>

namespace elle
{
  namespace radix
  {

    ///
    /// this class wraps a set of variables which are about to be
    /// initialized.
    ///
    /// the class provides methods, i.e Call() and Trigger(), for triggering a
    /// callback/closure/etc. with the variables.
    ///
    /// note that the difference with Arguments is that Arguments provide
    /// a constructor for initializing its constituent members, which is
    /// useful when these are references. on the other hand, the Variables
    /// class do not provide a constructor.
    ///
    template <typename... T>
    class Variables
    {
    };

    ///
    /// this specialized class is meaningless but is used to hold
    /// the Variables-related functionalities together.
    ///
    template <>
    class Variables<>
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
      Variables< Parameters<U..., V...> >
      Union(E1< Parameters<U...> >&,
            E2< Parameters<V...> >&);
    };

  }
}

#include <elle/radix/Variables.hxx>

#endif
