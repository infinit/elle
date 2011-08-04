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
// updated       julien quintard   [thu aug  4 11:25:32 2011]
//

#ifndef ETOILE_PATH_VENUE_HH
#define ETOILE_PATH_VENUE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <elle/idiom/Close.hh>
# include <vector>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a venue contains the addresses/versions corresponding to a route.
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
      typedef std::vector<nucleus::Location>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Venue();

      //
      // methods
      //
      elle::Status	Record(const nucleus::Address&,
			       const nucleus::Version&);

      elle::Status	Clear();

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
