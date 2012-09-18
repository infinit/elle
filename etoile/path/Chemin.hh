#ifndef ETOILE_PATH_CHEMIN_HH
# define ETOILE_PATH_CHEMIN_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/Size.hh>

# include <etoile/path/Route.hh>
# include <etoile/path/Venue.hh>

# include <elle/idiom/Close.hh>
#  include <limits>
# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace path
  {

    ///
    /// this class contains both a logical and physical path referred to
    /// as a chemin i.e path in French.
    ///
    /// note that for debugging purposes, the chemin embeds the route
    /// along with its associated venue.
    ///
    class Chemin:
      public elle::radix::Object
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
      Chemin(Route const& route,
             Venue const& venue);

      //
      // methods
      //
      elle::Status              Create(const Route&,
                                       const Venue&,
                                       const nucleus::neutron::Size =
                                         std::numeric_limits<
                                           nucleus::neutron::Size>::max());

      elle::Status              Derives(const Chemin&) const;

      elle::Status              Locate(nucleus::proton::Location&) const;

      //
      // interfaces
      //

      // object
      declare(Chemin);
      elle::Boolean             operator==(const Chemin&) const;
      elle::Boolean             operator<(const Chemin&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Route                     route;
      Venue                     venue;
    };

    std::ostream&
    operator << (std::ostream& stream, Chemin const& c);
  }
}

# include <etoile/path/Chemin.hxx>

#endif
