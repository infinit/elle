//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       path
//
// file          /home/mycure/infinit/etoile/path/Venue.hh
//
// created       julien quintard   [sat aug  8 15:56:09 2009]
// updated       julien quintard   [fri aug 14 21:11:55 2009]
//

#ifndef ETOILE_PATH_VENUE_HH
#define ETOILE_PATH_VENUE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

#include <vector>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a venue contains the addresses corresponding to a route.
    ///
    /// needless to say that a venue without its corresponding route.
    /// is useless.
    ///
    class Venue:
      public Dumpable
    {
    public:
      //
      // types
      //
      typedef std::vector<hole::Address>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // methods
      //

      // XXX

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Container		elements;
    };

  }
}

#endif
