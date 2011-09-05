//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Journal.hh
//
// created       julien quintard   [fri jun 17 13:04:03 2011]
// updated       julien quintard   [sun sep  4 17:54:52 2011]
//

#ifndef ETOILE_JOURNAL_JOURNAL_HH
#define ETOILE_JOURNAL_JOURNAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Scope.hh>

namespace etoile
{
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

#endif
