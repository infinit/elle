#include <QDataStream>

#include "hole/Key.hh"

namespace hole
{
  Key::Key()
  {
    quint32 * d = (quint32 *)data;
    quint32 * end = d + (sizeof (data) / 4);

    for (; d != end; d++)
      *d = qrand();
  }

  QDataStream &
  operator<<(QDataStream & ds, const Key & key)
  {
    ds.writeBytes((const char *)key.data, sizeof (key.data));
    return ds;
  }
}
