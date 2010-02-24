#include <QDataStream>

#include "hole/protocol/Header.hh"

namespace hole
{
  namespace protocol
  {
    QDataStream & operator<<(QDataStream & ds, const Header & header)
    {
      ds << header.id << header.tag << header.length;
      return ds;
    }

    QDataStream & operator>>(QDataStream & ds, Header & header)
    {
      ds >> header.id >> header.tag >> header.length;
      return ds;
    }

    QDataStream & operator<<(QDataStream & ds, const FindSuccessor & fs)
    {
      ds << fs.key;
      return ds;
    }

    QDataStream & operator>>(QDataStream & ds, FindSuccessor & fs)
    {
      ds >> fs.key;
      return ds;
    }
  }
}
