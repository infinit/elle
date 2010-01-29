//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/path/Venue.cc
//
// created       julien quintard   [sat aug  8 17:51:22 2009]
// updated       julien quintard   [fri jan 29 16:40:17 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Venue.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant is used a null definition.
    ///
    const Venue			Venue::Null;

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a venue.
    ///
    Status		Venue::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Venue::Scoutor	scoutor;

      std::cout << alignment << "[Venue]" << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
	   scoutor != this->elements.end();
	   scoutor++)
	if (scoutor->Dump(margin + 2) == StatusError)
	  escape("unable to dump the address");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Venue::operator==(const Venue&		element) const
    {
      Venue::Scoutor	s;
      Venue::Scoutor	t;

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

  }
}
