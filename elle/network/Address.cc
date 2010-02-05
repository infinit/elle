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
// updated       julien quintard   [wed feb  3 21:50:28 2010]
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

      std::cout << alignment << "[Address]" << std::endl;

      if (this->host.Dump(margin + 2) == StatusError)
	escape("unable to dump the host");

      std::cout << alignment << shift << "[Port] "
		<< std::dec << this->port << std::endl;

      leave();
    }

  }
}
