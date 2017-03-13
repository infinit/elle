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
    /// A Stream, in charge of multiplexing communications.
    ///
    /// When writing in a ChanneledStream, the packet is assigned to a Channel,
    /// uniquely identified. When writing the actual socket, the Channel in
    /// charge of the packet will identify the packet.
    /// On the other side of the socket, the other ChanneledStream will create a
    /// Channel with the same number, allowing the two owners of each side of
    /// the socket to communicate through the same socket. Multiplexing and
    /// demultiplexing will be transparent for the user.
    ///
    /// \code{.cc}
    ///
    /// // Consider two peer, connected by an arbitrary socket s.
    ///
    /// // Bob creates two Channels.
    /// elle::protocol::Serializer serializer(s);
    /// elle::protocol::ChanneledStream channel_stream(serializer);
    /// elle::protocol::Channel c1(channel_stream);
    /// elle::protocol::Channel c2(channel_stream);
    ///
    /// // Alice, on the other side can get those two Channels, via accept.
    /// elle::protocol::Serializer serializer(s);
    /// elle::protocol::ChanneledStream channel_stream(serializer);
    /// auto channel = channel_stream.accept();
    /// auto channel2 = channel_stream.accept();
    ///
    /// \endcode
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
      ChanneledStream(elle::reactor::Scheduler& scheduler, Stream& backend);
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
      ///
      elle::Buffer
      read() override;

      /// Wait for an incoming connection.
      ///
      /// The Channel
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
      ELLE_ATTRIBUTE(elle::reactor::Signal, channel_available);
      ELLE_ATTRIBUTE(Channel, default);
    };
  }
}

#endif
