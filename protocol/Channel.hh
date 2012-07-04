#ifndef INFINIT_PROTOCOL_CHANNEL_HH
# define INFINIT_PROTOCOL_CHANNEL_HH

# include <reactor/signal.hh>

# include <protocol/Stream.hh>
# include <protocol/fwd.hh>

namespace infinit
{
  namespace protocol
  {
    class Channel: public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Channel Self;
      typedef Stream Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Channel(ChanneledStream& backend);
      Channel(Channel&& source);
      ~Channel();
    private:
      Channel(ChanneledStream& backend, int id);

    /*---------.
    | Printing |
    `---------*/
    public:
      void print(std::ostream& s) const;

    /*-----------.
    | Properties |
    `-----------*/
    public:
      int id() const;

    /*----------.
    | Receiving |
    `----------*/
    public:
      virtual Packet read();

    /*--------.
    | Sending |
    `--------*/
    public:
      virtual void write(Packet& packet);

    /*--------.
    | Details |
    `--------*/
    private:
      friend class ChanneledStream;
      ChanneledStream& _backend;
      int _id;
      std::list<Packet> _packets;
      reactor::Signal _available;
    };

    // FIXME: printable interface
    std::ostream& operator << (std::ostream& s, Channel const& channel);
  }
}

#endif
