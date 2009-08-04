#include "hole/FullTag.hh"

namespace hole
{
  FullTag::FullTag()
    : tag(0),
      address(),
      port(0)
  {
  }

  FullTag::FullTag(protocol::Tag t,
                   QHostAddress a,
                   quint16 p)
    : tag(t),
      address(a),
      port(p)
  {
  }

  bool
  FullTag::operator<(const FullTag & other) const
  {
    if (tag < other.tag)
      return true;
    if (address.toString() < other.address.toString())
      return true;
    return port < other.port;
  }
}
