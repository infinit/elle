//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue jun 14 13:24:13 2011]
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
      static const Chemin                       Null;

      //
      // constructors & destructors
      //
      Chemin();

      //
      // methods
      //
      elle::Status              Create(const Route&,
                                       const Venue&,
                                       const nucleus::Size =
                                         elle::Type<nucleus::Size>::Maximum);

      elle::Status              Derives(const Chemin&) const;

      elle::Status              Locate(nucleus::Location&) const;

      //
      // interfaces
      //

      // object
      declare(Chemin);
      elle::Boolean             operator==(const Chemin&) const;
      elle::Boolean             operator<(const Chemin&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status              Serialize(elle::Archive&) const;
      elle::Status              Extract(elle::Archive&);

      //
      // attributes
      //
      Route                     route;
      Venue                     venue;
    };

  }
}

#endif
