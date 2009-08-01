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
// updated       julien quintard   [sat aug  1 23:17:58 2009]
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
      String		delimiter("/");
      std::vector<String> tokens;
      Integer32		i;

      // XXX
      String XXX("/suce/mon/cul");

      // split the path.
      if (Path::Split(XXX, delimiter, tokens) == StatusError)
	escape("unable to split the path");

      // look for the largest route in the cash.
      for (i = tokens.size(); i >= 0; i--)
	{
	  String	route("/");

	  // create a route by merging tokens from the longest to the smallest.
	  if (Path::Merge(tokens, i, route) == StatusError)
	    escape("unable to build a route");

	  // look in the cache.
	  if (Cache::Get(

	  ::sleep(3);
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Path::Split(const String&		path,
				    const String&		delimiter,
				    std::vector<String>&	tokens)
    {
      Natural32		start = path.find_first_not_of(delimiter);
      Natural32		end = path.find_first_of(delimiter, start);

      while ((start != std::string::npos) || (end != std::string::npos))
	{
	  tokens.push_back(path.substr(start, end - start));

	  start = path.find_first_not_of(delimiter, end);
	  end = path.find_first_of(delimiter, start);
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Path::Merge(const std::vector<String>&	tokens,
				    const Natural32		length,
				    String&			path)
    {
      std::vector<String>::const_iterator	iterator;
      Natural32					i;

      // go from 0 to length and append the tokens.
      for (iterator = tokens.begin(), i = 0;
	   (iterator != tokens.end()) && (i < length);
	   iterator++, i++)
	{
	  path.append(*iterator);

	  if (((iterator + 1) != tokens.end()) && ((i + 1) < length))
	    path.append("/");
	}

      leave();
    }

  }
}
