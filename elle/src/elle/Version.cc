#include <elle/Version.hh>

namespace elle
{

  /*-------------.
  | Construction |
  `-------------*/

  Version::Version():
    _major(0),
    _minor(0),
    _subminor(0)
  {}

  Version::Version(elle::Natural8 major,
                   elle::Natural8 minor,
                   elle::Natural8 subminor):
    _major(major),
    _minor(minor),
    _subminor(subminor)
  {}

  ELLE_SERIALIZE_CONSTRUCT_DEFINE(Version)
  {}

  /*----------.
  | Printable |
  `----------*/

  void
  Version::print(std::ostream& stream) const
  {
    stream << static_cast<elle::Natural32>(this->_major)
           << "."
           << static_cast<elle::Natural32>(this->_minor)
           << "."
           << static_cast<elle::Natural32>(this->_subminor);
  }

  /*----------.
  | Operators |
  `----------*/

  elle::Boolean
  Version::operator ==(Version const& other) const
  {
    return ((this->_major == other._major)
            && (this->_minor == other._minor)
            && (this->_subminor == other._subminor));
  }

  elle::Boolean
  Version::operator <(Version const& other) const
  {
    if (this->_major != other._major)
      return this->_major < other._major;
    if (this->_minor != other._minor)
      return this->_minor < other._minor;
    return this->_subminor < other._subminor;
  }

  elle::Boolean
  Version::operator >(Version const& other) const
  {
    return !((*this == other) || (*this < other));
  }

}
