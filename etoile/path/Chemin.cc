//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Chemin.cc
//
// created       julien quintard   [tue jun 14 22:25:57 2011]
// updated       julien quintard   [thu aug  4 12:39:30 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Chemin.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null chemin.
    ///
    const Chemin			Chemin::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Chemin::Chemin()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a chemin.
    ///
    elle::Status	Chemin::Create(const Route&		route,
				       const Venue&		venue,
				       const nucleus::Size	size)
    {
      enter();

      // clear the route because the chemin may have been used for
      // something else before.
      if (this->route.Clear() == elle::StatusError)
	escape("unable to clear the route");

      // do the same for the venue.
      if (this->venue.Clear() == elle::StatusError)
	escape("unable to clear the venue");

      //
      // import the route.
      //
      {
	Route::Scoutor	scoutor;
	nucleus::Size	i;

	// go through the route.
	for (scoutor = route.elements.begin(), i = 0;
	     (scoutor != route.elements.end()) && (i < size);
	     scoutor++, i++)
	  {
	    // record the route element.
	    this->route.elements.push_back(*scoutor);
	  }
      }

      //
      // import the venue.
      //
      {
	Venue::Scoutor	scoutor;
	nucleus::Size	i;

	// go through the venue.
	for (scoutor = venue.elements.begin(), i = 0;
	     (scoutor != venue.elements.end()) && (i < size);
	     scoutor++, i++)
	  {
	    // record the venue element.
	    this->venue.elements.push_back(*scoutor);
	  }
      }

      leave();
    }

    ///
    /// this method generates a Location based on the route and venue.
    ///
    elle::Status	Chemin::Locate(nucleus::Location&	location) const
    {
      enter();

      // check the size of the venue.
      if (this->venue.elements.size() == 0)
	escape("the venue seems to be empty");

      // set the location's attributes according to the venue last element.
      location.address =
	this->venue.elements[this->venue.elements.size() - 1].address;
      location.version =
	this->venue.elements[this->venue.elements.size() - 1].version;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Chemin::operator==(const Chemin&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the attributes..
      if ((this->route != element.route) ||
	  (this->venue != element.venue))
	false();

      true();
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Chemin::operator<(const Chemin&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	false();

      // compare the route only.
      if (this->route < element.route)
	true();

      false();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Chemin, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a chemin.
    ///
    elle::Status	Chemin::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Chemin]" << std::endl;

      // dump the route.
      if (this->route.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the route");

      // dump the venue.
      if (this->venue.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the venue");

      leave();
    }

  }
}
