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
// updated       julien quintard   [sat aug  8 18:30:44 2009]
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
// ---------- methods ---------------------------------------------------------
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

  }
}
