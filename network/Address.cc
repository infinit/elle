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
// updated       julien quintard   [thu mar 25 17:42:14 2010]
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

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal values.
      if ((this->host != element.host) ||
	  (this->port != element.port))
	false();

      true();
    }

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Address);

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the address.
    ///
    Status		Address::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the host and port.
      if (archive.Serialize(this->host,
			    this->port) == StatusError)
	escape("unable to serialize the address attributes");

      leave();
    }

    ///
    /// this method extracts an address.
    ///
    Status		Address::Extract(Archive&		archive)
    {
      enter();

      // extract the address.
      if (archive.Extract(this->host,
			  this->port) == StatusError)
	escape("unable to extract the address attributes");

      leave();
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

      enter();

      std::cout << alignment << "[Address]" << std::endl;

      if (this->host.Dump(margin + 2) == StatusError)
	escape("unable to dump the host");

      std::cout << alignment << Dumpable::Shift << "[Port] "
		<< std::dec << this->port << std::endl;

      leave();
    }

  }
}
