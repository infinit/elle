//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Journal.hh
//
// created       julien quintard   [sat jan 30 15:18:59 2010]
// updated       julien quintard   [sat jan 30 17:28:54 2010]
//

#ifndef ETOILE_JOURNAL_JOURNAL_HH
#define ETOILE_JOURNAL_JOURNAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>
#include <etoile/context/Context.hh>

#include <list>

namespace etoile
{

  ///
  /// XXX
  ///
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
      static Status	Initialize();
      static Status	Clean();

      static Status	Push(context::Context*);
      static Status	Pop(context::Context*&);

      static Status	Get(const hole::Address&,
			    hole::Block*&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Set.hh>

#endif
