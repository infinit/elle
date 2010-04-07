//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Item.hh
//
// created       julien quintard   [mon apr  5 21:15:34 2010]
// updated       julien quintard   [tue apr  6 11:43:36 2010]
//

#ifndef ETOILE_JOURNAL_ITEM_HH
#define ETOILE_JOURNAL_ITEM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Block.hh>

#include <etoile/journal/Operation.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a block in the journal.
    ///
    class Item:
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Item(hole::Address*);
      Item(hole::Block*);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Operation		operation;

      union
      {
	hole::Address*	address;
	hole::Block*	block;
      };
    };

  }
}

#endif
