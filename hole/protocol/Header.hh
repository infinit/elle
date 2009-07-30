#ifndef HEADER_HH
# define HEADER_HH

namespace hole
{
  namespace protocol
  {
    typedef quint16 CmdId;
    typedef quint16 Tag;
    typedef quint16 CmdLen;

    struct Header
    {
      CmdId  id;
      Tag    tag;
      CmdLen length;
    };
  }
}

#endif /* !HEADER_HH */
