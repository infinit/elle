#include "hole/FullTag.hh"

namespace hole
{
  FullTag::FullTag(protocol::Tag t,
                   QHostAddress a,
                   quint16 p)
    : tag(t),
      address(a),
      port(p)
  {
  }
}
