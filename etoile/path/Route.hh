#ifndef ETOILE_PATH_ROUTE_HH
# define ETOILE_PATH_ROUTE_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <etoile/path/fwd.hh>
# include <etoile/path/Slab.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

namespace etoile
{
  namespace path
  {

    ///
    /// a route is a sequence of slabs forming a path, each slab representing
    /// the name of subdirectory down to the target object along with their
    /// version numbers.
    ///
    /// note that this class also contains the version number of the root
    /// directory. indeed, the first slab is always used for representing
    /// the root directory even though its slab is empty.
    ///
    class Route:
      public elle::radix::Object
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

      //
      // methods
      //
      elle::Status              Create(const Way&);
      elle::Status              Create(const Route&,
                                       const Slab&);

      elle::Status              Derives(const Route&) const;

      elle::Status              Clear();

      //
      // interfaces
      //

      // object
      declare(Route);
      elle::Boolean             operator==(const Route&) const;
      elle::Boolean             operator<(const Route&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container                 elements;
    };

  }
}

# include <etoile/path/Route.hxx>

#endif
