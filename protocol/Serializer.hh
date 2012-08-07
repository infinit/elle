#ifndef INFINIT_PROTOCOL_PACKET_SERIALIZER_HH
# define INFINIT_PROTOCOL_PACKET_SERIALIZER_HH

# include <iostream>

# include <reactor/mutex.hh>

# include <protocol/Stream.hh>

namespace infinit
{
  namespace protocol
  {
    class Serializer: public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Stream Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Serializer(reactor::Scheduler& scheduler, std::iostream& stream);

    /*----------.
    | Receiving |
    `----------*/
    public:
      Packet read();

    /*--------.
    | Sending |
    `--------*/
    public:
      void write(Packet& packet);

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual void print(std::ostream& stream) const;

    /*--------.
    | Details |
    `--------*/
    private:
      std::iostream& _stream;
      reactor::Mutex _lock_write;
      reactor::Mutex _lock_read;
    };
  }
}

#endif
