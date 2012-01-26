//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue jun 14 12:57:24 2011]
//
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Path.hh>

#include <etoile/path/Path.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes a string-based path i.e a way and resolves it
    /// into a location.
    ///
    /// note that the way may contain version indicators for Etoile to
    /// use a specific version of the named directory, file etc. contained
    /// in the path.
    ///
    elle::Status        Path::Resolve(
                          const path::Way&                      way,
                          path::Chemin&                         chemin)
    {
      path::Route       route;
      path::Venue       venue;

      enter();

      // debug.
      if (Infinit::Configuration.etoile.debug == true)
        printf("[etoile] wall::Path::Resolve()\n");

      // create a route from the way.
      if (route.Create(way) == elle::StatusError)
        escape("unable to create the route");

      // resolve the way.
      if (path::Path::Resolve(route, venue) == elle::StatusError)
        escape("unable to resolve the path");

      // create the chemin.
      if (chemin.Create(route, venue) == elle::StatusError)
        escape("unable to create the chemin");

      leave();
    }

    ///
    /// this method takes an system absolute path _way_ and verifies that
    /// it belongs to the current Infinit file system. if so, the relative
    /// path is returned.
    ///
    /// for example, assuming Infinit runs on the local mountpoint
    /// /home/mycure/local/mnt/infinit/ and given the _way_
    /// /home/mycure/local/mnt/infinit/tmp/teton.txt, this method would
    /// return a _path_ /tmp/teton.txt.
    ///
    elle::Status        Path::Locate(
                          const path::Way&                      absolute,
                          path::Way&                            relative)
    {
      enter();

      // verify that the given way lies in the mountpoint.
      if (absolute.path.find(Infinit::Mountpoint) == elle::String::npos)
        escape("the given way is not located in this Infinit file system");

      // extract the relative path.
      relative.path = absolute.path.substr(Infinit::Mountpoint.length());

      // if the relative path is empty, just return the root directory.
      if (relative.path.empty() == true)
        relative.path = elle::System::Path::Root;

      leave();
    }

  }
}
