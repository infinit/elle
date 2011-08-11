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
// updated       julien quintard   [wed aug 10 23:28:57 2011]
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
    /// XXX
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
      elle::Status	Add(Riffle*);
      elle::Status	Remove(Riffle*);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container		container;
    };

  }
}

#endif
