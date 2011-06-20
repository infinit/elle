//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Chemin.hh
//
// created       julien quintard   [tue jun 14 13:24:13 2011]
// updated       julien quintard   [thu jun 16 11:01:00 2011]
//

#ifndef ETOILE_PATH_CHEMIN_HH
#define ETOILE_PATH_CHEMIN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/path/Route.hh>
#include <etoile/path/Venue.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains both a logical and physical path referred to
    /// as a chemin i.e path in French.
    ///
    /// note that for debugging purposes, the chemin embeds the route
    /// along with its associated venue.
    ///
    class Chemin:
      elle::Object
    {
    public:
      //
      // constants
      //
      static const Chemin			Null;

      //
      // methods
      //
      elle::Status	Create(const Route&,
			       const Venue&);

      elle::Status	Locate(nucleus::Location&);

      //
      // interfaces
      //

      // XXX

      //
      // attributes
      //
      Route		route;
      Venue		venue;
    };

  }
}

#endif
