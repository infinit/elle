#ifndef ETOILE_PATH_VENUE_HH
# define ETOILE_PATH_VENUE_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Location.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace path
  {

    ///
    /// this class contains the addresses/versions corresponding to a route.
    ///
    /// a venue is therefore composed of a sequence of Location, each
    /// of which indicates the address and version number of the component.
    ///
    class Venue:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Venue                Null;

      //
      // types
      //
      typedef std::vector<nucleus::proton::Location> Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // constructors & destructors
      //
      Venue();

      //
      // methods
      //
      elle::Status              Record(const nucleus::proton::Location&);
      elle::Status              Record(const nucleus::proton::Address&,
                                       const nucleus::proton::Version&);

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

      //
      // attributes
      //
      Container                 elements;
    };

  }
}

# include <etoile/path/Venue.hxx>

#endif
