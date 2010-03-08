//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Host.cc
//
// created       julien quintard   [fri oct 16 05:24:44 2009]
// updated       julien quintard   [tue mar  2 15:10:11 2010]
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

      // compare the internal values.
      if ((this->type != element.type) ||
	  (this->location != element.location))
	false();

      true();
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
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Host]" << std::endl;

      std::cout << alignment << shift << "[Type] "
		<< this->type << std::endl;

      std::cout << alignment << shift << "[Location] "
		<< this->location.toString().toStdString() << std::endl;

      leave();
    }

  }
}
