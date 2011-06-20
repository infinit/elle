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
// updated       julien quintard   [thu jun 16 11:05:09 2011]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a chemin.
    ///
    elle::Status	Chemin::Create(const Route&		route,
				       const Venue&		venue)
    {
      enter();

      // set the attributes.
      this->route = route;
      this->venue = venue;

      leave();
    }

    ///
    /// this method generates a Location based on the route and venue.
    ///
    elle::Status	Chemin::Locate(nucleus::Location&	location)
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

  }
}
