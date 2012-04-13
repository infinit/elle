//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [wed sep  7 12:37:56 2011]
//

#ifndef HORIZON_LINUX_JANITOR_HH
#define HORIZON_LINUX_JANITOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <etoile/Etoile.hh>

namespace horizon
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

#include <horizon/linux/Janitor.hxx>

#endif
