#ifndef HEADER_HH
# define HEADER_HH

# include <QtGlobal>

# include "hole/Key.hh"

class QDataStream;

namespace hole
{
  namespace protocol
  {
    typedef ::quint16 CmdId;
    typedef quint64 Tag;
    typedef quint16 CmdLen;

    struct Header
    {
      enum CommandId
      {
        Ping = 0,
        Pong = 1,
        Quit = 2,
        FindSuccessor = 3,
        SuccessorFound = 4,
      };

      CmdId  id;
      Tag    tag;
      CmdLen length;
    };

    QDataStream & operator<<(QDataStream &, const Header &);
    QDataStream & operator>>(QDataStream &, Header &);

    struct FindSuccessor
    {
      Header header;
      Key    key;

      /// Compute the header's length field
      inline static size_t Length() { return sizeof (key); }
    };
    QDataStream & operator<<(QDataStream &, const FindSuccessor &);
    QDataStream & operator>>(QDataStream &, FindSuccessor &);
  }
}

#endif /* !HEADER_HH */
