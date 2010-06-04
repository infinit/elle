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
// updated       julien quintard   [fri may 28 17:49:28 2010]
//

#ifndef ETOILE_PATH_VENUE_HH
#define ETOILE_PATH_VENUE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

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
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Venue		Null;

      //
      // types
      //
      typedef std::vector<hole::Address>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // methods
      //
      elle::Status	Record(const hole::Address&);

      //
      // interfaces
      //

      // object
      declare(Venue);
      elle::Boolean	operator==(const Venue&) const;

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container		elements;
    };

  }
}

#endif
