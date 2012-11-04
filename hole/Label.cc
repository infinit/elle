#include <hole/Label.hh>

namespace hole
{
  /*----------.
  | Constants |
  `----------*/

  elle::cryptography::oneway::Algorithm const Label::Algorithms::oneway(
    elle::cryptography::oneway::Algorithm::sha256);

  /*-------------.
  | Construction |
  `-------------*/

  Label::Label()
  {
  }

  /*----------.
  | Operators |
  `----------*/

  elle::Boolean
  Label::operator ==(Label const& other) const
  {
    if (this == &other)
      return (true);

    return (this->_digest == other._digest);
  }

  elle::Boolean
  Label::operator <(Label const& other) const
  {
    if (this == &other)
      return (false);

    return (this->_digest < other._digest);
  }

  elle::Boolean
  Label::operator <=(Label const& other) const
  {
    if (this == &other)
      return (true);

    return (this->_digest <= other._digest);
  }

  /*---------.
  | Dumpable |
  `---------*/

  elle::Status          Label::Dump(elle::Natural32             margin) const
  {
    elle::String        alignment(margin, ' ');

    // display the name.
    std::cout << alignment << "[Label]" << std::endl;

    if (this->_digest.Dump(margin + 2) == elle::Status::Error)
      escape("unable to dump the digest");

    return elle::Status::Ok;
  }

}
