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
// updated       julien quintard   [thu may  5 16:03:29 2011]
//

#ifndef ETOILE_JOURNAL_ITEM_HH
#define ETOILE_JOURNAL_ITEM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

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
      Item(const nucleus::Address&,
	   nucleus::Block*);
      Item(const nucleus::Address&);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Operation		operation;

      nucleus::Address	address;

      nucleus::Block*	block;
    };

  }
}

#endif
