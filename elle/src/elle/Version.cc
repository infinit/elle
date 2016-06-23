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
        *this = Version::from_string(str);
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

  Version
  Version::from_string(std::string const& repr)
  {
    if (repr.size() == 0)
      throw elle::serialization::Error("version format error: empty");

    std::vector<std::string> strs;
    boost::split(strs, repr, boost::is_any_of("."));
    std::string major = strs.size() > 0 ? strs[0] : "0";
    std::string minor = strs.size() > 1 ? strs[1] : "0";
    std::string sub_minor = strs.size() > 2 ? strs[2] : "0";

    try
    {
      Version v(std::stoi(major),
              std::stoi(minor),
              std::stoi(sub_minor));
      return v;
    }
    catch (std::invalid_argument const& e)
    {
      throw elle::serialization::Error(
        elle::sprintf("version format error: %s", e.what()));
    }
  }

  std::string
  Version::to_string(elle::Version const& version)
  {
    return elle::sprintf("%s.%s.%s",
                         version.major(),
                         version.minor(),
                         version.subminor());
  }
}

/*-----.
| Hash |
`-----*/


namespace std
{
  std::size_t
  hash<elle::Version>::operator()(elle::Version const& version) const
  {
    return
      (std::size_t(version.major()) << 16) |
      (std::size_t(version.minor()) << 8) |
      (std::size_t(version.subminor()));
  }
}
