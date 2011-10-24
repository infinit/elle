//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri jun 17 13:04:03 2011]
//

#ifndef ETOILE_JOURNAL_JOURNAL_HH
#define ETOILE_JOURNAL_JOURNAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs to be forward-declared in order to prevent
    /// conflicts.
    ///
    class Scope;

  }

  ///
  /// this namespace contains everything related to the journal which
  /// is responsible for recording and triggering the storage layer operations.
  ///
  namespace journal
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the journal manager.
    ///
    class Journal
    {
    public:
      //
      // static methods
      //
      static elle::Status	Record(gear::Scope*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Scope.hh>

#endif
