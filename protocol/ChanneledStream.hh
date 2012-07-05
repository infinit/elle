#ifndef INFINIT_PROTOCOL_CHANNELEDSTREAM_HH
# define INFINIT_PROTOCOL_CHANNELEDSTREAM_HH

# include <unordered_map>

# include <protocol/Channel.hh>
# include <protocol/Stream.hh>
# include <protocol/fwd.hh>

namespace infinit
{
  namespace protocol
  {
    class ChanneledStream: public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef ChanneledStream Self;
      typedef Stream Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      ChanneledStream(reactor::Scheduler& scheduler, Stream& backend, bool master);

    /*----.
    | IDs |
    `----*/
    private:
      int  _id_generate();
      bool _master;
      int  _id_current;

    /*----------.
    | Receiving |
    `----------*/
    public:
      virtual Packet read();
      Channel accept();
    private:
      Packet _read(Channel* channel);
      bool _reading;

    /*--------.
    | Sending |
    `--------*/
    public:
      virtual void write(Packet& packet);
    private:
      void _write(Packet& packet, int id);

    /*--------.
    | Details |
    `--------*/
    private:
      Stream& _backend;
      friend class Channel;
      std::unordered_map<int, Channel*> _channels;
      std::list<Channel> _channels_new;
      reactor::Signal _channel_available;
      Channel _default;
    };
  }
}

#endif
