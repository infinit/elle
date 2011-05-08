//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/Network.cc
//
// created       julien quintard   [mon feb 16 21:42:37 2009]
// updated       julien quintard   [sun may  8 02:07:12 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Network.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable can easily be used for comparing with invalid networks.
    ///
    const Network		Network::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Network::Network()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// create a network object based on its name.
    ///
    elle::Status	Network::Create(const elle::String&	name)
    {
      enter();

      // assign the name.
      this->name = name;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Network::operator==(const Network&	element) const
    {
      enter();

      // check the network as this may actually be the same object.
      if (this == &element)
	true();

      // compare the names.
      if (this->name != element.name)
	false();

      true();
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean	Network::operator<(const Network&	element) const
    {
      enter();

      // check the network as this may actually be the same object.
      if (this == &element)
	false();

      // compare the names.
      if (this->name < element.name)
	false();

      false();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Network, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an network object.
    ///
    elle::Status	Network::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      // display the network's name.
      std::cout << alignment << "[Network] "
		<< this->name << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the network object.
    ///
    elle::Status	Network::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the internal.
      if (archive.Serialize(this->name) == elle::StatusError)
	escape("unable to serialize the name");

      leave();
    }

    ///
    /// this method extracts the network object.
    ///
    elle::Status	Network::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the internal.
      if (archive.Extract(this->name) == elle::StatusError)
	escape("unable to extract the name");

      leave();
    }

  }
}
