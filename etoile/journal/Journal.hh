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
// updated       julien quintard   [tue apr  6 12:12:24 2010]
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
      static Status	Initialize();
      static Status	Clean();

      static Status	Record(context::Context*);

      static Status	Get(const hole::Address&,
			    hole::Block*&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Bucket.hh>
#include <etoile/journal/Operation.hh>

#include <etoile/hole/Hole.hh>

#endif
