#include <elle/Version.hh>

namespace elle
{

  //
  // ---------- construction ----------------------------------------------------
  //

  Version::Version():
    _major(0),
    _minor(0)
  {
  }

  Version::Version(elle::Natural8 major,
                   elle::Natural8 minor):
    _major(major),
    _minor(minor)
  {
  }

  //
  // ---------- methods ---------------------------------------------------------
  //

  elle::Natural8
  Version::major() const
  {
    return (this->_major);
  }

  elle::Natural8
  Version::minor() const
  {
    return (this->_minor);
  }

  //
  // ---------- printable -------------------------------------------------------
  //

  void
  Version::print(std::ostream& stream) const
  {
    stream << static_cast<elle::Natural32>(this->_major)
           << ":"
           << static_cast<elle::Natural32>(this->_minor);
  }

  //
  // ---------- operators -------------------------------------------------------
  //

  elle::Boolean
  Version::operator ==(Version const& other) const
  {
    return ((this->_major == other._major) && (this->_minor == other._minor));
  }

  elle::Boolean
  Version::operator !=(Version const& other) const
  {
    return (!this->operator ==(other));
  }

  elle::Boolean
  Version::operator <(Version const& other) const
  {
    return ((this->_major < other._major) ||
            ((this->_major == other._major) && (this->_minor < other._minor)));
  }

  elle::Boolean
  Version::operator >(Version const& other) const
  {
    return ((this->_major > other._major) ||
            ((this->_major == other._major) && (this->_minor > other._minor)));
  }

  elle::Boolean
  Version::operator <=(Version const& other) const
  {
    return (!this->operator >(other));
  }

  elle::Boolean
  Version::operator >=(Version const& other) const
  {
    return (!this->operator <(other));
  }

}
