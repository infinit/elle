#include <elle/Version.hh>
#include <elle/serialization/Serializer.hh>
#include <boost/algorithm/string.hpp>
#include <string>

ELLE_LOG_COMPONENT("elle.Version");

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

  /*--------------.
  | Serialization |
  `--------------*/

  void
  Version::serialize(elle::serialization::Serializer& s)
  {
    ELLE_DEBUG("Version: serialize");
    if (s.text())
    {
      ELLE_DEBUG("Version: is text(json,xml,etc.)");
      std::stringstream ss;
      print(ss);
      std::string str = ss.str();
      s.serialize_forward(str);
      if (s.in())
      {
        ELLE_DEBUG("Version: deserializing");
        std::vector<std::string> strs;
        boost::split(strs, str, boost::is_any_of("."));
        this->_major = std::stoi(strs[0]);
        this->_minor = std::stoi(strs[1]);
        this->_subminor = std::stoi(strs[2]);
      }
      else
        ELLE_DEBUG("Version: serializing");
    }
    else
    {
      ELLE_DEBUG("Version: is binary");
      s.serialize("major", this->_major);
      s.serialize("minor", this->_minor);
      s.serialize("subminor", this->_subminor);
    }
  }
}
