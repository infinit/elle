//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Bucket.hh
//
// created       julien quintard   [sat jan 30 16:26:50 2010]
// updated       julien quintard   [tue apr  6 11:49:45 2010]
//

#ifndef ETOILE_JOURNAL_BUCKET_HH
#define ETOILE_JOURNAL_BUCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Address.hh>
#include <etoile/hole/Block.hh>

#include <etoile/journal/Item.hh>

#include <vector>

namespace etoile
{
  namespace journal
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// the bucket is used to collect the objects, hence blocks, that must
    /// be published or destroyed.
    ///
    struct Bucket
    {
    public:
      //
      // types
      //
      typedef std::vector<Item*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // methods
      //
      Status		Record(hole::Block*);
      Status		Record(hole::Address*);

      //
      // attributes
      //
      Container		container;
    };

  }
}

#endif
