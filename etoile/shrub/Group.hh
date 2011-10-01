//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [sat sep 10 16:03:39 2011]
//

#ifndef ETOILE_SHRUB_GROUP_HH
#define ETOILE_SHRUB_GROUP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/shrub/Riffle.hh>

#include <elle/idiom/Close.hh>
# include <vector>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace shrub
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a set of riffles grouped together because
    /// they have been created or refreshed at the exact same time.
    ///
    /// note that a very simple data structure is being used in this
    /// class because no more than a dozen riffles are stored in a single
    /// group.
    ///
    class Group:
      public elle::Entity
    {
    public:
      //
      // types
      //
      typedef std::vector<Riffle*>		Container;
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
