#ifndef HOLE_FULLTAG_HH
# define HOLE_FULLTAG_HH

# include <QHostAddress>

# include "hole/protocol/Header.hh"

namespace hole
{
  class RemoteNode;

  struct FullTag
  {
    FullTag();
    FullTag(protocol::Tag tag,
            QHostAddress  address,
            quint16       port);

    protocol::Tag tag;
    QHostAddress  address;
    quint16       port;

    bool operator<(const FullTag & other) const;
    bool operator==(const FullTag & other) const;
  };
}

#endif /* !HOLE_FULLTAG_HH */
