//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Address.cc
//
// created       julien quintard   [sat nov 28 13:01:48 2009]
// updated       julien quintard   [fri aug 12 16:25:39 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Address.hh>

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
    /// this method creates an address from a string of the form
    /// 'host:port'.
    ///
    Status		Address::Create(const String&		string)
    {
      Natural32		separator;

      enter();

      // locate the ':' separator.
      separator = string.find_first_of(':');

      // create the host.
      if (this->host.Create(string.substr(0, separator)) == StatusError)
	escape("unable to create the host");

      // create the port.
      this->port = ::strtoul(string.substr(separator + 1).c_str(), NULL, 0);

      leave();
    }

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
// ---------- object ----------------------------------------------------------
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
    /// compare two objects.
    ///
    Boolean		Address::operator<(const Address&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	false();

      // compare the host.
      if (this->host < element.host)
	true();
      else if (this->host > element.host)
	false();

      // compare the port.
      if (this->port < element.port)
	true();
      else if (this->port > element.port)
	false();

      false();
    }

    ///
    /// compare two objects.
    ///
    Boolean		Address::operator>(const Address&	element) const
    {
      return (!(this->operator<=(element)));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Address, _());

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
