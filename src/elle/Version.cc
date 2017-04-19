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

  Version::Version()
    : Version{0, 0, 0}
  {}

  /*----------.
  | Printable |
  `----------*/

  void
  Version::print(std::ostream& stream) const
  {
    stream << static_cast<uint32_t>(this->_major)
           << "."
           << static_cast<uint32_t>(this->_minor)
           << "."
           << static_cast<uint32_t>(this->_subminor);
  }

  /*----------.
  | Operators |
  `----------*/

  bool
  Version::operator ==(Version const& other) const
  {
    return std::tie(this->_major, this->_minor, this->_subminor)
      == std::tie(other._major, other._minor, other._subminor);
  }

  bool
  Version::operator <(Version const& other) const
  {
    return std::tie(this->_major, this->_minor, this->_subminor)
      < std::tie(other._major, other._minor, other._subminor);
  }

  bool
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
      ELLE_DEBUG("Version: is text (json, xml, etc.)");
      auto str = elle::sprintf("%s", *this);
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
    if (repr.empty())
      throw elle::serialization::Error("version format error: empty");

    auto strs = std::vector<std::string>{};
    boost::split(strs, repr, boost::is_any_of("."));
    auto const major    = strs.size() > 0 ? strs[0] : "0";
    auto const minor    = strs.size() > 1 ? strs[1] : "0";
    auto const subminor = strs.size() > 2 ? strs[2] : "0";

    try
    {
      // FIXME: not checking it fits in int8.
      return Version(std::stoi(major), std::stoi(minor), std::stoi(subminor));
    }
    catch (std::invalid_argument const& e)
    {
      throw elle::serialization::Error(
       elle::sprintf("version format error: %s (%s)", e.what(), repr));
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
