#ifndef INFINIT_PROTOCOL_PACKET_HH
# define INFINIT_PROTOCOL_PACKET_HH

# include <memory>

namespace infinit
{
  namespace protocol
  {
    class Packet
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      typedef unsigned char Byte;
      typedef unsigned int Size;
      Packet(Byte* data, Size data_size);
      Packet(Packet&& source);
      ~Packet();

    /*-----------.
    | Interfaces |
    `-----------*/
    public:
      std::unique_ptr<std::istream> stream() const;

    /*--------.
    | Details |
    `--------*/
    private:
      Packet(Size data_size);
      Packet(const Packet&);
      friend class PacketStream;
    public: // FIXME
      Byte* _data;
      unsigned int _data_size;
    };
  }
}

#endif
