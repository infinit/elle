#ifndef ETOILE_PATH_PATH_HH
# define ETOILE_PATH_PATH_HH

#include <elle/types.hh>
#include <nucleus/proton/fwd.hh>

#include <etoile/path/fwd.hh>

namespace etoile
{
  ///
  /// this namespace contains everything necessary for resolving paths.
  ///
  namespace path
  {

    ///
    /// this class is the entry point to path resolution.
    ///
    class Path
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      static elle::Status       Resolve(const Route&,
                                        Venue&);
      static elle::Status       Parse(const Slab&,
                                      Slice&,
                                      nucleus::proton::Revision&);
    };

  }
}

#endif
