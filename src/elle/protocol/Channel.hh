#ifndef ELLE_PROTOCOL_CHANNEL_HH
# define ELLE_PROTOCOL_CHANNEL_HH

# include <elle/Printable.hh>

# include <elle/reactor/signal.hh>

# include <elle/protocol/Stream.hh>
# include <elle/protocol/fwd.hh>

namespace elle
{
  namespace protocol
  {
    /// A Channel of a ChanneledStream, identified by its unique Id.
    ///
    /// The Channels are specialization of Stream, designed to work with a
    /// ChanneledStream as backend. When writing in a Channel, the packet will
    /// be prefixed by the Id of the Channel allowing the ChanneledStream to
    /// determine the destination of the packet through a single connection.
    ///
    /// @see ChanneledStream for more information.
    ///
    /// \code{.cc}
    ///
    /// // Consider channeled_stream, a ChanneledStream.
    ///
    /// // Open a Channel.
    /// elle::protocol::Channel channel1(channeled_stream);
    /// channel1.write({"hello there!"});
    ///
    /// // Wait and connect to a Channel open by the peer.
    /// auto channel2 = channeled_stream.accept();
    /// auto packet = channel2.read();
    ///
    /// \endcode
    class Channel
      : public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      using Self = Channel;
      using Super = Stream;
      using Id = int;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Construct a Channel for a ChannelStream.
      ///
      /// @param backend The ChannelStream used as backend.
      Channel(ChanneledStream& backend);
      /// Move constructor.
      Channel(Channel&& source);
      ~Channel();
    private:
      /// Construct a Channel for a ChanneledStream.
      ///
      /// @param backend The ChanneledStream used as backend.
      /// @param id The id of the Channel.
      Channel(ChanneledStream& backend, int id);

    /*--------.
    | Version |
    `--------*/
    public:
      ELLE_attribute_r(elle::Version, version, override);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& s) const override;

    /*----------.
    | Receiving |
    `----------*/
    public:
      /// Read data from the Channel.
      ///
      /// @see Stream::read.
      elle::Buffer
      read() override;

    /*--------.
    | Sending |
    `--------*/
    protected:
      /// Write data to the Channel.
      ///
      /// @see Stream::_write.
      void
      _write(elle::Buffer const& packet) override;

    /*--------.
    | Details |
    `--------*/
    private:
      friend class ChanneledStream;
      ELLE_ATTRIBUTE(ChanneledStream&, backend);
      ELLE_ATTRIBUTE_R(Id, id);
      ELLE_ATTRIBUTE(std::list<elle::Buffer>, packets);
      ELLE_ATTRIBUTE(elle::reactor::Signal, available);
    };
  }
}

#endif
