//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Set.hh
//
// created       julien quintard   [sat jan 30 16:26:50 2010]
// updated       julien quintard   [sat jan 30 16:39:40 2010]
//

#ifndef ETOILE_JOURNAL_SET_HH
#define ETOILE_JOURNAL_SET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Hole.hh>

#include <list>

namespace etoile
{
  namespace journal
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// XXX
    ///
    struct Set
    {
    public:
      //
      // types
      //
      typedef std::list<hole::Block*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;
    };

  }
}

#endif
