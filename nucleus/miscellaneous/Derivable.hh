//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri jul  8 10:42:21 2011]
//

#ifndef NUCLEUS_MISCELLANEOUS_DERIVABLE_HH
#define NUCLEUS_MISCELLANEOUS_DERIVABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <elle/package/Derivable.hh>
#include <nucleus/Nucleus.hh>

namespace elle
{
  using namespace package;
}

namespace nucleus
{
  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a nucleus-specific derivable class i.e a
    /// class for generating nucleus objects based on a product number.
    ///
    template <typename T>
    class Derivable:
      public elle::Derivable<T>
    {
    public:
      //
      // constructors & destructors
      //
      Derivable();
      Derivable(const elle::Product&,
                const T&);
      Derivable(T&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/miscellaneous/Derivable.hxx>

#endif
