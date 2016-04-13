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
      Serializer(std::iostream& stream, bool checksum = true);
      Serializer(reactor::Scheduler& scheduler, std::iostream& stream,
                 bool checksum = true);

    /*----------.
    | Receiving |
    `----------*/
    public:
      elle::Buffer
      read();

    /*--------.
    | Sending |
    `--------*/
    protected:
      void
      _write(elle::Buffer const& packet) override;

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const override;

    /*--------.
    | Details |
    `--------*/
      ELLE_ATTRIBUTE(std::iostream&, stream, protected);
      ELLE_ATTRIBUTE(reactor::Mutex, lock_write, protected);
      ELLE_ATTRIBUTE(reactor::Mutex, lock_read, protected);
      ELLE_ATTRIBUTE(bool, checksum, protected);
    };
  }
}

#endif
