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
// updated       julien quintard   [sun sep  4 18:14:56 2011]
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
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Chemin			Null;

      //
      // constructors & destructors
      //
      Chemin();

      //
      // methods
      //
      elle::Status		Create(const Route&,
				       const Venue&,
				       const nucleus::Size =
				         elle::Type<nucleus::Size>::Maximum);

      elle::Status		Locate(nucleus::Location&) const;

      //
      // interfaces
      //

      // object
      declare(Chemin);
      elle::Boolean		operator==(const Chemin&) const;
      elle::Boolean		operator<(const Chemin&) const;

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Route			route;
      Venue			venue;
    };

  }
}

#endif
