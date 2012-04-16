//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       path
//
// author        julien quintard   [sat aug  8 15:56:09 2009]
//

#ifndef ETOILE_PATH_VENUE_HH
#define ETOILE_PATH_VENUE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
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
    /// this class contains the addresses/versions corresponding to a route.
    ///
    /// a venue is therefore composed of a sequence of Location, each
    /// of which indicates the address and version number of the component.
    ///
    class Venue:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Venue                Null;

      //
      // types
      //
      typedef std::vector<nucleus::Location>    Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // constructors & destructors
      //
      Venue();

      //
      // methods
      //
      elle::Status              Record(const nucleus::Location&);
      elle::Status              Record(const nucleus::Address&,
                                       const nucleus::Version&);

      elle::Status              Derives(const Venue&) const;

      elle::Status              Clear();

      //
      // interfaces
      //

      // object
      declare(Venue);
      elle::Boolean             operator==(const Venue&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status              Serialize(elle::Archive&) const;
      //elle::Status              Extract(elle::Archive&);

      //
      // attributes
      //
      Container                 elements;
    };

  }
}

#endif
