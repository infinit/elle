#include <nucleus/proton/Network.hh>

#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    Network::Network()
    {
    }

    Network::Network(elle::String const& name):
      _name(name)
    {
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Network::operator ==(Network const& other) const
    {
      // check the network as this may actually be the same object.
      if (this == &other)
        return true;

      return (this->_name == other._name);
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Network::print(std::ostream& stream) const
    {
      stream << this->_name;
    }

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
                << this->_name << std::endl;

      return elle::Status::Ok;
    }

  }
}
