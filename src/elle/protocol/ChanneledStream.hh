#ifndef ELLE_PROTOCOL_CHANNELEDSTREAM_HH
# define ELLE_PROTOCOL_CHANNELEDSTREAM_HH

# include <unordered_map>

# include <elle/protocol/Channel.hh>
# include <elle/protocol/Stream.hh>
# include <elle/protocol/fwd.hh>

namespace elle
{
  namespace protocol
  {
    class ChanneledStream
      : public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef ChanneledStream Self;
      typedef Stream Super;
      typedef std::unordered_map<int, Channel*> Channels;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      ChanneledStream(reactor::Scheduler& scheduler, Stream& backend);
      ChanneledStream(Stream& backend);

    /*--------.
    | Version |
    `--------*/
      ELLE_attribute_r(elle::Version, version, override);

    /*----.
    | IDs |
    `----*/
    private:
      ELLE_ATTRIBUTE(bool, master);
      ELLE_ATTRIBUTE(int, id_current);
    private:
      int
      _id_generate();

      bool
      _handshake(Stream& backend);

    /*----------.
    | Receiving |
    `----------*/
    public:
      elle::Buffer
      read() override;

      Channel
      accept();

    private:
      /// Read and dispatch packets until one that fit our needs is found.
      ///
      /// @param new_channel Whether to return when and only when a new channel
      ///                    is opened.
      /// @param channel Id of the channel receiving a packet on will cause the
      ///                function to return. Ignored if new_channel is true.
      void
      _read(bool new_channel, int channel);

      elle::Buffer
      _read(Channel* channel);

      ELLE_ATTRIBUTE(bool, reading);

    /*--------.
    | Sending |
    `--------*/
    protected:
      void
      _write(elle::Buffer const& packet) override;
    private:
      void
      _write(elle::Buffer const& packet, int id);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const override;

    /*--------.
    | Details |
    `--------*/
    private:
      friend class Channel;

      ELLE_ATTRIBUTE(Stream&, backend);
      ELLE_ATTRIBUTE(Channels, channels);
      ELLE_ATTRIBUTE(std::list<Channel>, channels_new);
      ELLE_ATTRIBUTE(reactor::Signal, channel_available);
      ELLE_ATTRIBUTE(Channel, default);
    };
  }
}

#endif
