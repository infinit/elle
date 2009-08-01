#include <QDataStream>

#include "hole/Key.hh"

namespace hole
{
  QDataStream & operator<<(QDataStream & ds, const Key & key)
  {
    ds.writeBytes((const char *)key.data, sizeof (key.data));
    return ds;
  }
}
