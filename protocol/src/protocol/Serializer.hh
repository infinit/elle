#ifndef INFINIT_PROTOCOL_PACKET_SERIALIZER_HH
# define INFINIT_PROTOCOL_PACKET_SERIALIZER_HH

# include <iostream>

# include <reactor/mutex.hh>

# include <protocol/Stream.hh>
# include <elle/attribute.hh>

namespace infinit
{
  namespace protocol
  {
    class Serializer
      : public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Stream Super;
      typedef std::iostream Inner;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Serializer(std::iostream& stream,
                 bool checksum = true);

      Serializer(reactor::Scheduler& scheduler,
                 std::iostream& stream,
                 bool checksum = true);

    protected:
      Serializer(elle::Version const& version,
                 reactor::Scheduler& scheduler,
                 std::iostream& stream,
                 bool checksum);

    public:
      ~Serializer();

    /*----------.
    | Receiving |
    `----------*/
    public:
      elle::Buffer
      read() override;

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
      ELLE_ATTRIBUTE_R(elle::Version, version);
    protected:
      ELLE_ATTRIBUTE(Inner&, stream);
      ELLE_ATTRIBUTE(reactor::Mutex, lock_write, protected);
      ELLE_ATTRIBUTE(reactor::Mutex, lock_read, protected);
      ELLE_ATTRIBUTE_R(bool, checksum, protected);
      ELLE_ATTRIBUTE_R(elle::Buffer::Size, chunk_size);

    // public:
    //   class pImpl;
    // private:
    //   std::unique_ptr<pImpl> _impl;
    };
  }
}

#endif
