//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/path/Venue.cc
//
// created       julien quintard   [sat aug  8 17:51:22 2009]
// updated       julien quintard   [thu aug  4 11:26:10 2011]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Venue::Venue()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records the next step of the venue.
    ///
    elle::Status	Venue::Record(const nucleus::Address&	address,
				      const nucleus::Version&	version)
    {
      nucleus::Location	location;

      enter();

      // create the location.
      if (location.Create(address, version) == elle::StatusError)
	escape("unable to create the location");

      // store the address in the container.
      this->elements.push_back(location);

      leave();
    }

    ///
    /// this method clears the venue's content.
    ///
    elle::Status	Venue::Clear()
    {
      enter();

      // clear the container.
      this->elements.clear();

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Venue::operator==(const Venue&		element) const
    {
      Venue::Scoutor	s;
      Venue::Scoutor	t;

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
    embed(Venue, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a venue.
    ///
    elle::Status	Venue::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');
      Venue::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Venue] " << std::dec
		<< this->elements.size() << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
	   scoutor != this->elements.end();
	   scoutor++)
	if (scoutor->Dump(margin + 2) == elle::StatusError)
	  escape("unable to dump the address");

      leave();
    }

  }
}
