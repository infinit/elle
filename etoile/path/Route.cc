//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Route.cc
//
// created       julien quintard   [sat aug  8 16:26:41 2009]
// updated       julien quintard   [mon jul  4 12:41:22 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Path.hh>
#include <etoile/path/Route.hh>

#include <agent/Agent.hh>

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
    /// note that the first slab is always empty in order to represent
    /// the root directory.
    ///
    /// the following assumes the root version indicator is '@' while the
    /// slab version indicator is '%'.
    ///
    /// note that the ways can embed version numbers as shown next:
    ///
    ///   /suce%42/avale/leche.txt%3
    ///
    /// however, the format '%[0-9]+' cannot be used with the root directory
    /// since the way always starts with '/...'.
    ///
    /// in order to provide this functionality, the following check is made:
    /// if the first non-empty slab starts with '@[0-9]+', then this slab is
    /// used as the root one with the appropriate version number.
    ///
    elle::Status	Route::Create(const Way&		way)
    {
      Length		start;
      Length		end;
      Slab		slab;

      enter();

      // check that the way starts with a leading '/'
      if (way.path[0] != elle::System::Path::Separator)
	escape("the path must contain the leading path separator '%c'",
	       elle::System::Path::Separator);

      // clear the elements.
      this->elements.clear();

      // compute the next offsets.
      start =
	way.path.find_first_not_of(elle::System::Path::Separator);
      end =
	way.path.find_first_of(elle::System::Path::Separator, start);

      // check if at least one slab is present.
      if (start < way.path.length())
	{
	  // create the slab.
	  slab = way.path.substr(start, end - start);

	  // check if the slab represents the root directory i.e starts
	  // with '@' and follows with a possible version number.
	  if (slab[0] == agent::Agent::Configuration.history.indicator.root)
	    {
	      // modify the '@' character with the version indicator '%'.
	      slab[0] = agent::Agent::Configuration.history.indicator.slab;

	      // record the slab.
	      this->elements.push_back(slab);

	      // compute the next offsets.
	      start =
		way.path.find_first_not_of(elle::System::Path::Separator, end);
	      end =
		way.path.find_first_of(elle::System::Path::Separator, start);
	    }
	}

      // if no slab is present or the first slab does not represent the
      // root directory---i.e the elements container is empty---register
      // an empty root slab.
      if (this->elements.empty() == true)
	{
	  // record an empty root slab.
	  this->elements.push_back("");
	}

      // then, go through the string.
      while (start < way.path.length())
	{
	  // create the slab.
	  slab = way.path.substr(start, end - start);

	  // add the section to the container.
	  this->elements.push_back(slab);

	  // compute the next offsets.
	  start =
	    way.path.find_first_not_of(elle::System::Path::Separator, end);
	  end =
	    way.path.find_first_of(elle::System::Path::Separator, start);
	}

      leave();
    }

    ///
    /// this method creates a route by appending a name to an existing route.
    ///
    elle::Status	Route::Create(const Route&		route,
				      const Slab&		slab)
    {
      enter();

      // copy the elements.
      this->elements = route.elements;

      // add the slab.
      this->elements.push_back(slab);

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Route::operator==(const Route&		element) const
    {
      Route::Scoutor	s;
      Route::Scoutor	t;

      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the size.
      if (this->elements.size() != element.elements.size())
	false();

      // for every element.
      for (s = this->elements.begin(), t = element.elements.begin();
	   s != this->elements.end();
	   s++, t++)
	if (*s != *t)
	  false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Route, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a route.
    ///
    elle::Status	Route::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      Route::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Route] " << std::dec
		<< this->elements.size() << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
	   scoutor != this->elements.end();
	   scoutor++)
	std::cout << alignment << elle::Dumpable::Shift
		  << *scoutor << std::endl;

      leave();
    }

  }
}
