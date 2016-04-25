#ifndef INFINIT_PROTOCOL_PACKET_SERIALIZER_HH
# define INFINIT_PROTOCOL_PACKET_SERIALIZER_HH

# include <iostream>

# include <reactor/mutex.hh>

# include <protocol/Stream.hh>

# ifdef EOF
#  undef EOF
# endif

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
      class EOF
        : public elle::Error
      {
      public:
        EOF();
      };

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
      elle::Buffer read();

    /*--------.
    | Sending |
    `--------*/
    protected:
      virtual
      void
      _write(elle::Buffer& packet);

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual void print(std::ostream& stream) const;

    /*--------.
    | Details |
    `--------*/
    private:
      ELLE_ATTRIBUTE_RX(std::iostream&, stream);
      reactor::Mutex _lock_write;
      reactor::Mutex _lock_read;
      bool           _checksum;
    };
  }
}

#endif
