//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/Path.cc
//
// created       julien quintard   [sat aug  1 21:27:28 2009]
// updated       julien quintard   [sun aug  9 14:20:57 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/util/Path.hh>

namespace pig
{
  namespace util
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this methods resolves a path into an object address.
    ///
    /// first, the method tries looking for the largest matching path in the
    /// cache. then, for the remaining steps, the directories and retrieved,
    /// access verifies etc.
    ///
    /// therefore, it is interesting to note that, as long as an entry is
    /// in cache, it bypass the potential access control changes.
    ///
    Status		Path::Resolve(const String&		path,
				      Address&			address)
    {
      std::list<String>::iterator iterator;
      std::list<String>		segments;
      String			delimiter("/");
      String			route;
      Address			location;

      // split the path into a cached route and a list of unresolved tokens.
      if (Path::Split(path, delimiter, route, segments) == StatusError)
	escape("unable to split the path");

      // retrieve the largest cached address.
      if (Cache::Get(route, location) == StatusError)
	escape("unable to retrieve the address of the cached path");

      if (segments.size() != 0)
	{
	  // resolve all the other segments.
	  for (iterator = segments.begin();
	       iterator != segments.end();
	       iterator++)
	    {
	      etoile::core::Object	directory;
	      String			segment = *iterator;

	      // load the segment directory.
	      if (Hole::Get(location, directory) == StatusError)
		escape("unable to load the directory");

	      // lookup for the segment.
	      if (Directory::Lookup(directory,
				    segment,
				    location) == StatusError)
		escape("unable to lookup for the segment");

	      // XXX[put in cache]
	    }
	}

      // set the address of the final object.
      address = location;

      leave();
    }

    ///
    /// this method takes a path and a delimiter and splits the path
    /// into two parts. The first part is called 'route' and is the largest
    /// leftmost part of the path that is contained in cache. The second
    /// part, called 'segments' is a list of uncached names that need to
    /// be manually resolved by retrieving the directories, looking up the
    /// entry etc.
    ///
    Status		Path::Split(const String&		path,
				    const String&		delimiter,
				    String&			route,
				    std::list<String>&		segments)
    {
      Integer32		end = path.find_last_not_of(delimiter);
      Integer32		start = path.find_last_of(delimiter, end);

      while (true)
	{
	  String	segment;

	  segment = path.substr(start + 1, end - start);
	  route = path.substr(0, start);

	  if (segment.length() != 0)
	    segments.push_front(segment);

	  if (route.length() == 0)
	    route = delimiter;

	  if (Cache::Search(route) == StatusTrue)
	    break;

	  end = path.find_last_not_of(delimiter, start);
	  start = path.find_last_of(delimiter, end);
	}

      leave();
    }

    ///
    /// this method returns both the leftmost part and the smallest right
    /// part of the given path.
    ///
    Status		Path::Break(const String&		path,
				    const String&		delimiter,
				    String&			directory,
				    String&			object)
    {
      Integer32		end = path.find_last_not_of(delimiter);
      Integer32		start = path.find_last_of(delimiter, end);

      // XXX[handle weird cases such as /suce or /teton/avale/]

      directory = path.substr(0, start);
      object = path.substr(start + 1, end - start);

      if (directory.length() == 0)
	directory = delimiter;

      leave();
    }

  }
}
