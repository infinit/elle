//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Route.cc
//
// created       julien quintard   [sat aug  8 16:26:41 2009]
// updated       julien quintard   [sat aug  8 22:23:01 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Path.hh>
#include <etoile/path/Route.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a route from a string by splitting it according
    /// to the path separator.
    ///
    Status		Route::Create(const String&		path)
    {
      Natural32		start;
      Natural32		end;

      // initialize the pointers.
      start = path.find_first_not_of(Path::Separator);
      end = path.find_first_of(Path::Separator, start);

      while (true)
	{
	  String	element;

	  // locate the next element.
	  element = path.substr(start, end - start);

	  // if the element exist, add it to the container.
	  this->elements.push_back(element);

	  // compute the next offsets.
	  start = path.find_first_not_of(Path::Separator, end);
	  end = path.find_first_of(Path::Separator, start);

	  // quit if the end is reached.
	  if (start >= path.length())
	    break;
	}

      leave();
    }

    ///
    /// this method dumps a route.
    ///
    Status		Route::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Route::Scoutor	scoutor;

      std::cout << alignment << "[Route]" << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
	   scoutor != this->elements.end();
	   scoutor++)
	std::cout << alignment << shift << *scoutor << std::endl;

      leave();
    }

  }
}
