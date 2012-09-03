#ifndef ETOILE_WALL_PATH_HH
# define ETOILE_WALL_PATH_HH

# include <elle/types.hh>

# include <reactor/exception.hh>

# include <etoile/path/fwd.hh>
# include <etoile/path/Way.hh>

namespace etoile
{
  namespace wall
  {
    /// Path resolving into locations.
    class Path
    {
    public:
      /// Resolve a string-based path i.e a way into a location.
      ///
      /// The way may contain version indicators for Etoile to use a
      /// specific version of the named directory, file etc. contained
      /// in the path.
      static
      path::Chemin
      resolve(const path::Way&);
    };

    class NoSuchFileOrDirectory: public reactor::Exception
    {
    public:
      NoSuchFileOrDirectory(reactor::Scheduler& sched, path::Way const& path);
      ~NoSuchFileOrDirectory() throw ();
    private:
      path::Way _path;
    };
  }
}

#endif
