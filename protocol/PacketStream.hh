#ifndef INFINIT_PROTOCOL_PACKET_STREAM_HH
# define INFINIT_PROTOCOL_PACKET_STREAM_HH

# include <iostream>

# include <protocol/Packet.hh>

namespace infinit
{
  namespace protocol
  {
    class PacketStream
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      PacketStream(std::iostream& stream);

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

    /*--------.
    | Details |
    `--------*/
    private:
      std::iostream& _stream;
    };
  }
}

#endif
