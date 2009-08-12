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

  int
  Key::Cmp(const Key & other) const
  {
    if (&other == this)
      return 0;

    quint32 * d1 = (quint32 *)data;
    quint32 * d2 = (quint32 *)other.data;
    quint32 * d1_end = d1 + sizeof (data) / sizeof (*d1);

    for (; d1 < d1_end; d1++, d2++)
    {
      if (*d1 > *d2)
        return 1;
      if (*d1 < *d2)
        return -1;
    }
    return 0;
  }

  QDataStream &
  operator<<(QDataStream & ds, const Key & key)
  {
    ds.writeBytes((const char *)key.data, sizeof (key.data));
    return ds;
  }

  QDataStream &
  operator>>(QDataStream & ds, Key & key)
  {
    if (ds.readRawData((char *)key.data, sizeof (key.data))
      != sizeof (key.data))
      ds.setStatus(QDataStream::ReadCorruptData);
    return ds;
  }
}
