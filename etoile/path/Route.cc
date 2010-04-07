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
// updated       julien quintard   [wed apr  7 00:58:05 2010]
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
    Status		Route::Create(const Way&		way)
    {
      Length		start;
      Length		end;

      enter();

      // initialize the pointers.
      start = way.path.find_first_not_of(System::Path::Separator);
      end = way.path.find_first_of(System::Path::Separator, start);

      // go through the string.
      while (start < way.path.length())
	{
	  Slice		element;

	  // locate the next element.
	  element = way.path.substr(start, end - start);

	  // if the element exist, add it to the container.
	  this->elements.push_back(element);

	  // compute the next offsets.
	  start = way.path.find_first_not_of(System::Path::Separator, end);
	  end = way.path.find_first_of(System::Path::Separator, start);
	}

      leave();
    }

    ///
    /// this method dumps a route.
    ///
    Status		Route::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Route::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Route] " << std::dec
		<< this->elements.size() << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
	   scoutor != this->elements.end();
	   scoutor++)
	std::cout << alignment << Dumpable::Shift << *scoutor << std::endl;

      leave();
    }

  }
}
