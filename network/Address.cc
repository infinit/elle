//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Address.cc
//
// created       julien quintard   [fri oct 16 05:24:44 2009]
// updated       julien quintard   [mon nov 30 00:52:18 2009]
//

#include <elle/network/Address.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Address		Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor initializes the object to Null.
    ///
    Address::Address():
      type(TypeNull)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method enables the caller the creation of special addresses.
    ///
    Status		Address::Create(const Type		type)
    {
      // set the type.
      this->type = type;

      // set the address accordingly.
      switch (this->type)
	{
	case TypeNull:
	case TypeAny:
	  {
	    this->address.clear();

	    break;
	  }
	case TypeBroadcast:
	  {
	    this->address = ::QHostAddress::Broadcast;

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
    Status		Address::Create(const String&		string)
    {
      if (this->address.setAddress(string.c_str()) == false)
	escape("unable to set the address");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method assigns an address to another.
    ///
    Address&		Address::operator=(const Address&	element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the address.
      if (this->Recycle<Address>(&element) == StatusError)
	yield("unable to recycle the address", *this);

      return (*this);
    }

    ///
    /// checks if two objects match.
    ///
    Boolean		Address::operator==(const Address&	element) const
    {
      // compare the internal values.
      if ((this->type != element.type) ||
	  (this->address != element.address))
	false();

      true();
    }

    ///
    /// checks if two objects dis-match.
    ///
    Boolean		Address::operator!=(const Address&	element) const
    {
      return (!(*this == element));
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

      std::cout << alignment << shift << "[Type] "
		<< this->type << std::endl;

      std::cout << alignment << shift << "[Address] "
		<< this->address.toString().toStdString() << std::endl;

      leave();
    }

  }
}
