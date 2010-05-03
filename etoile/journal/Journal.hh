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
// updated       julien quintard   [mon may  3 13:11:56 2010]
//

#ifndef ETOILE_JOURNAL_JOURNAL_HH
#define ETOILE_JOURNAL_JOURNAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Block.hh>

#include <etoile/context/Context.hh>

namespace etoile
{
  ///
  /// the journal namespace contains classes which are used for buffering
  /// the modifications, hence acting as a cache at the same time.
  ///
  namespace journal
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class receives blocks to publish or destroy from the
    /// components classes while responding to the depot looking for
    /// a particular block.
    ///
    class Journal
    {
    public:
      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Record(context::Context*);

      static elle::Status	Get(const hole::Address&,
				    hole::Block*&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Bucket.hh>
#include <etoile/journal/Item.hh>
#include <etoile/journal/Operation.hh>

#endif
