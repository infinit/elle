//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [mon feb 16 21:42:37 2009]
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
    const Network               Network::Null;

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
    elle::Status        Network::Create(const elle::String&     name)
    {
      ;

      // assign the name.
      this->name = name;

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Network::operator==(const Network&      element) const
    {
      ;

      // check the network as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // compare the names.
      if (this->name != element.name)
        return elle::StatusFalse;

      return elle::StatusTrue;
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Network::operator<(const Network&       element) const
    {
      ;

      // check the network as this may actually be the same object.
      if (this == &element)
        return elle::StatusFalse;

      // compare the names.
      if (this->name < element.name)
        return elle::StatusFalse;

      return elle::StatusFalse;
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
    elle::Status        Network::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      ;

      // display the network's name.
      std::cout << alignment << "[Network] "
                << this->name << std::endl;

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the network object.
    ///
    elle::Status        Network::Serialize(elle::Archive&       archive) const
    {
      ;

      // serialize the internal.
      if (archive.Serialize(this->name) == elle::StatusError)
        escape("unable to serialize the name");

      return elle::StatusOk;
    }

    ///
    /// this method extracts the network object.
    ///
    elle::Status        Network::Extract(elle::Archive&         archive)
    {
      ;

      // extract the internal.
      if (archive.Extract(this->name) == elle::StatusError)
        escape("unable to extract the name");

      return elle::StatusOk;
    }

  }
}
