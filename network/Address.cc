//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Address.cc
//
// created       julien quintard   [sat nov 28 13:01:48 2009]
// updated       julien quintard   [fri mar  5 15:08:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Address.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the definition of a Null address.
    ///
    const Address		Address::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Address::Address():
      port(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates an address.
    ///
    Status		Address::Create(const Host&		host,
					const Port		port)
    {
      enter();

      this->host = host;
      this->port = port;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// checks if two objects match.
    ///
    Boolean		Address::operator==(const Address&	element) const
    {
      enter();

      // compare the internal values.
      if ((this->host != element.host) ||
	  (this->port != element.port))
	false();

      true();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an address.
    ///
    Status		Address::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Address]" << std::endl;

      if (this->host.Dump(margin + 2) == StatusError)
	escape("unable to dump the host");

      std::cout << alignment << shift << "[Port] "
		<< std::dec << this->port << std::endl;

      leave();
    }

  }
}
