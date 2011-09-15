//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 16 05:24:44 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Host.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Host			Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor initializes the object to Null.
    ///
    Host::Host():
      type(TypeNull)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method enables the caller the creation of special hosts.
    ///
    Status		Host::Create(const Type		type)
    {
      enter();

      // set the type.
      this->type = type;

      // set the host accordingly.
      switch (this->type)
	{
	case TypeNull:
	case TypeAny:
	  {
	    this->location = ::QHostAddress::Any;

	    break;
	  }
	case TypeBroadcast:
	  {
	    this->location = ::QHostAddress::Broadcast;

	    break;
	  }
	case TypeIP:
	  {
	    // nothing to do here.
	  }
	}

      leave();
    }

    ///
    /// this method creates an IPv4 or IPv6 address.
    ///
    Status		Host::Create(const String&		string)
    {
      enter();

      // set the type.
      this->type = Host::TypeIP;

      // set the address.
      if (this->location.setAddress(string.c_str()) == false)
	escape("unable to set the location");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// checks if two objects match.
    ///
    Boolean		Host::operator==(const Host&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the internal values.
      if ((this->type != element.type) ||
	  (this->location != element.location))
	false();

      true();
    }

    ///
    /// compare two objects.
    ///
    Boolean		Host::operator<(const Host&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	false();

      // compare the type
      if (this->type < element.type)
	true();
      else if (this->type > element.type)
	false();
      else
	{
	  ::QString	first;
	  ::QString	second;

	  // generate the string.
	  first = this->location.toString();
	  second = element.location.toString();

	  // compare the string.
	  if (first < second)
	    true();
	  else if (first > second)
	    false();
	}

      false();
    }

    ///
    /// compare two objects.
    ///
    Boolean		Host::operator>(const Host&		element) const
    {
      return (!(this->operator<=(element)));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Host, _());

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the host.
    ///
    Status		Host::Serialize(Archive&		archive) const
    {
      String		host(this->location.toString().toStdString());

      enter();

      // serialize the host and port.
      if (archive.Serialize(static_cast<Natural8>(this->type),
			    host) == StatusError)
	escape("unable to serialize the host");

      leave();
    }

    ///
    /// this method extracts a host.
    ///
    Status		Host::Extract(Archive&			archive)
    {
      Natural8		type;
      String		host;

      enter();

      // extract the host.
      if (archive.Extract(type, host) == StatusError)
	escape("unable to extract the host");

      // set the type.
      this->type = static_cast<Host::Type>(type);

      // set the location.
      this->location.setAddress(host.c_str());

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an host.
    ///
    Status		Host::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Host]" << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Type] "
		<< this->type << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Location] "
		<< this->location.toString().toStdString() << std::endl;

      leave();
    }

  }
}
