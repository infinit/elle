#pragma once

#include <iostream>

#include <elle/reactor/mutex.hh>

#include <elle/attribute.hh>
#include <elle/compiler.hh>

#include <elle/protocol/Stream.hh>

#ifdef EOF
# undef EOF
#endif

namespace elle
{
  namespace protocol
  {
    /// A serializer that wrap a std::iostream and is in charge of:
    /// - negotiating the version of the protocol.
    /// - splitting packets into small chunks.
    /// - write and read control bytes (e. Transfer is interrupted without
    ///   closing the connection).
    /// - ensuring data integrity (via a checksum).
    /// - etc.
    ///
    /// When a serializer is constructed on top a std::iostream, it will push
    /// its version and read the peer version in order to agree what version to
    /// use (actually, the smaller of the versions).
    ///
    /// \code{.cc}
    ///
    /// elle::reactor::network::TCPSocket socket("127.0.0.1", 8182);
    /// elle::protocol::Serializer serializer(socket, elle::Version{0, 3, 0},
    ///                                       true);
    /// // On top of that Serializer, you can create a ChanneledStream.
    /// elle::protocol::ChanneledStream cstream(serializer);
    ///
    /// \endcode
    class ELLE_API Serializer
      : public Stream
    {
    /*------.
    | Types |
    `------*/
    public:
      using Super = Stream;
      using Inner = std::iostream;
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
      /// Construct a Serializer.
      ///
      /// @param stream The underlying std::iostream.
      /// @param version The version of the protocol.
      /// @param checksum Whether it should read and write the checksum of
      ///                 packets sent.
      Serializer(std::iostream& stream,
                 elle::Version const& version = elle::Version(0, 1, 0),
                 bool checksum = true);
      /// Construct a Serializer.
      ///
      /// @param scheduler The Scheduler to use.
      /// @param stream The underlying std::iostream.
      /// @param version The version of the protocol.
      /// @param checksum Whether it should read and write the checksum of
      ///                 packets sent.
      Serializer(elle::reactor::Scheduler& scheduler,
                 std::iostream& stream,
                 elle::Version const& version  = elle::Version(0, 1, 0),
                 bool checksum = true);

    public:
      ~Serializer();

    /*----------.
    | Receiving |
    `----------*/
    public:
      /// Read a complete packet from the underlying stream.
      ///
      /// @returns A buffer containing the data sent by the peer.
      elle::Buffer
      read() override;

    /*--------.
    | Sending |
    `--------*/
    protected:
      /// Write data to the stream.
      ///
      /// @param packet The packet to write.
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
      ELLE_ATTRIBUTE_RX(std::iostream&, stream);
      ELLE_ATTRIBUTE_R(elle::Version, version, override);
      ELLE_ATTRIBUTE_R(elle::Buffer::Size, chunk_size);
      ELLE_ATTRIBUTE_R(bool, checksum);
    public:
      class pImpl;
    private:
      ELLE_ATTRIBUTE(std::unique_ptr<pImpl>, impl);
    };
  }
}
