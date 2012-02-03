//
// ---------- header ----------------------------------------------------------
//
// project       facade
//
// license       infinit
//
// author        julien quintard   [wed sep  7 12:37:56 2011]
//

#ifndef FACADE_LINUX_JANITOR_HH
#define FACADE_LINUX_JANOTIR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>

namespace facade
{
  namespace linux
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the janitor is responsible for discarding the identifiers it is
    /// given.
    ///
    /// the janitor is called whenever an unexpected error occurs in the
    /// crux.
    ///
    class Janitor
    {
    public:
      //
      // static methods
      //
      static elle::Void         Clear();

      template <typename T>
      static elle::Void         Clear(T&);
      template <typename T,
                typename... TT>
      static elle::Void         Clear(T&,
                                      TT&...);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <facade/linux/Janitor.hxx>

#endif
