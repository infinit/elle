#include <nucleus/proton/Clef.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Clef::Clef(Address const& address,
               cryptography::SecretKey const& secret):
      _address(address),
      _secret(secret)
    {
    }

    Clef::Clef(Clef const& other):
      _address(other._address),
      _secret(other._secret)
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEF(Clef,
                                 _address, _secret)
    {
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Clef::print(std::ostream& stream) const
    {
      stream << this->_address
             << ", "
             << this->_secret;
    }
  }
}
