//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/shrub/Queue.hh
//
// created       julien quintard   [tue aug  9 18:17:59 2011]
// updated       julien quintard   [sun sep  4 20:03:08 2011]
//

#ifndef ETOILE_SHRUB_QUEUE_HH
#define ETOILE_SHRUB_QUEUE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/shrub/Riffle.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a data structures fo holding timestamps
    /// and references to their associated riffles.
    ///
    /// the queue is used for keeping the riffle depending on the
    /// refreshing timestamps so that, given a lifespan, the expired
    /// ones may get evicted.
    ///
    class Queue:
      public elle::Entity
    {
    public:
      //
      // types
      //
      typedef std::pair<elle::Time, Riffle*>	Value;
      typedef std::map<elle::Time, Riffle*>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // methods
      //
      elle::Status		Add(Riffle*);
      elle::Status		Remove(Riffle*);

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container			container;
    };

  }
}

#endif
