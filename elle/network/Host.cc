//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Host.cc
//
// created       julien quintard   [fri oct 16 05:24:44 2009]
// updated       julien quintard   [wed mar 31 21:18:42 2010]
//

#include <elle/network/Host.hh>

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

      // set the address.
      if (this->location.setAddress(::QString(string.c_str())) == false)
	escape("unable to set the location");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
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
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Host);

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
      if (archive.Serialize((Natural8&)this->type,
			    host) == StatusError)
	escape("unable to serialize the host");

      leave();
    }

    ///
    /// this method extracts a host.
    ///
    Status		Host::Extract(Archive&			archive)
    {
      String		host;

      enter();

      // extract the host.
      if (archive.Extract((Natural8&)this->type,
			  host) == StatusError)
	escape("unable to extract the host");

      // create the host properly.
      if (this->Create(host) == StatusError)
	escape("unable to create the host");

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
