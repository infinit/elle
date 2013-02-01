#ifndef ETOILE_PATH_ROUTE_HH
# define ETOILE_PATH_ROUTE_HH

# include <elle/types.hh>
# include <elle/operator.hh>

# include <etoile/path/fwd.hh>
# include <etoile/path/Slab.hh>

# include <vector>

namespace etoile
{
  namespace path
  {

    ///
    /// a route is a sequence of slabs forming a path, each slab representing
    /// the name of subdirectory down to the target object along with their
    /// revision numbers.
    ///
    /// note that this class also contains the revision number of the root
    /// directory. indeed, the first slab is always used for representing
    /// the root directory even though its slab is empty.
    ///
    class Route
    {
    public:
      //
      // constants
      //
      static const Route                Null;
      static Route                      Root;

      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      //
      // types
      //
      typedef std::vector<Slab>                 Container;
      typedef Container::iterator               Iterator;
      typedef Container::const_iterator         Scoutor;

      //
      // constructors & destructors
      //
      Route();
      Route(Route const&) = default;

      //
      // methods
      //
      elle::Status              Create(const Way&);
      elle::Status              Create(const Route&,
                                       const Slab&);

      elle::Boolean             Derives(const Route&) const;

      elle::Status              Clear();

      //
      // interfaces
      //

      ELLE_OPERATOR_ASSIGNMENT(Route); // XXX

      elle::Boolean             operator==(const Route&) const;
      elle::Boolean             operator<(const Route&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container                 elements;
    };

    std::ostream&
    operator << (std::ostream& stream, Route const& r);
  }
}

# include <etoile/path/Route.hxx>

#endif
