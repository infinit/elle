#include <nucleus/proton/Network.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/idiom/Open.hh>

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
      // assign the name.
      this->name = name;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Network::operator==(const Network&      element) const
    {
      // check the network as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->name == element.name);
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

      // display the network's name.
      std::cout << alignment << "[Network] "
                << this->name << std::endl;

      return elle::Status::Ok;
    }

  }
}
