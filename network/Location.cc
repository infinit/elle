//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Location.cc
//
// created       julien quintard   [sat nov 28 13:01:48 2009]
// updated       julien quintard   [sun nov 29 03:28:06 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Location.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Location::Location():
      port(0)
    {
    }

    ///
    /// this constructor has been introduced for commodity has it cannot
    /// return an error.
    ///
    Location::Location(const Address&				address,
		       const Natural16				port)
    {
      this->address = address;
      this->port = port;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an address very much like the constructor.
    ///
    Status		Location::Create(const Address&		address,
					 const Natural16	port)
    {
      this->address = address;
      this->port = port;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method assigns an address to another.
    ///
    Location&		Location::operator=(const Location&	element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the address.
      if (this->Recycle<Location>(&element) == StatusError)
	yield("unable to recycle the address", *this);

      return (*this);
    }

    ///
    /// checks if two objects match.
    ///
    Boolean		Location::operator==(const Location&	element) const
    {
      // compare the internal values.
      if ((this->address != element.address) ||
	  (this->port != element.port))
	false();

      true();
    }

    ///
    /// checks if two objects dis-match.
    ///
    Boolean		Location::operator!=(const Location&	element) const
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an address.
    ///
    Status		Location::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Location]" << std::endl;

      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

      std::cout << alignment << shift << "[Port] "
		<< std::dec << this->port << std::endl;

      leave();
    }

  }
}
