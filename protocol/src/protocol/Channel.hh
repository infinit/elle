#ifndef INFINIT_PROTOCOL_CHANNEL_HH
# define INFINIT_PROTOCOL_CHANNEL_HH

# include <elle/Printable.hh>

# include <reactor/signal.hh>

# include <protocol/Stream.hh>
# include <protocol/fwd.hh>

namespace infinit
{
  namespace protocol
  {
    class Channel
      : public Stream
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

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual void print(std::ostream& s) const;

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
    protected:
      virtual
      void
      _write(Packet& packet);

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
  }
}

#endif
