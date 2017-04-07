#pragma once

#include <unordered_map>

#include <elle/protocol/Channel.hh>
#include <elle/protocol/Stream.hh>
#include <elle/protocol/fwd.hh>

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
      using Self = ChanneledStream;
      using Super = Stream;
      using Channels = std::unordered_map<int, Channel*>;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      ChanneledStream(elle::reactor::Scheduler& scheduler, Stream& backend);
      ChanneledStream(Stream& backend);
      virtual
      ~ChanneledStream();
    private:
      void
      _read_thread();
      ELLE_ATTRIBUTE(Stream&, backend);
      ELLE_ATTRIBUTE(reactor::Thread::unique_ptr, thread);
      ELLE_ATTRIBUTE(std::exception_ptr, exception);

    /*--------.
    | Version |
    `--------*/
    public:
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
      /// Wait for an incoming connection.
      ///
      /// The Channel
      Channel
      accept();
    protected:
      elle::Buffer
      _read() override;

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
      ELLE_ATTRIBUTE(Channels, channels);
      ELLE_ATTRIBUTE(reactor::Channel<Channel>, channels_new);
      ELLE_ATTRIBUTE(elle::reactor::Signal, channel_available);
      ELLE_ATTRIBUTE(Channel, default);
    };
  }
}
