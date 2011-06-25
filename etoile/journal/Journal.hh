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
// updated       julien quintard   [fri jun 24 14:37:05 2011]
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
  namespace journal
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
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
