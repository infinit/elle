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
// updated       julien quintard   [mon may  3 17:19:23 2010]
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
      public elle::Entity
    {
    public:
      //
      // constructors & destructors
      //
      Item(const hole::Address&,
	   hole::Block*);
      Item(const hole::Address&);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Operation		operation;

      hole::Address	address;

      hole::Block*	block;
    };

  }
}

#endif
