#pragma once

#include <elle/Buffer.hh>
#include <elle/IOStream.hh>
#include <elle/attribute.hh>
#include <elle/reactor/asio.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/mutex.hh>
#include <elle/reactor/network/Protocol.hh>
#include <elle/reactor/network/fwd.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      template <typename AsioSocket>
      class SocketOperation;

      /// The abstract base class for Sockets used in the reactor.
      ///
      /// Class inheriting from Socket must implement:
      /// - write
      /// - read_some
      /// - read_until
      /// - close
      class Socket
        : public elle::IOStream
      {
      /*------.
      | Types |
      `------*/
      public:
        /// Self type.
        using Self = Socket;

      /*----------.
      | Constants |
      `----------*/
      public:
        static size_t const buffer_size;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Create an unbound socket.
        Socket();
        /// Destroy a socket.
        ~Socket() override;
        /// Create a Socket for the given protocol.
        ///
        /// @param protocol The transport protocl to use.
        /// @param hostname The host name.
        /// @param port The The port the host is listen to.
        /// @param connection_timeout The maximum duration before the connection
        ///                           attempt times out.
        /// @returns Freshly connected Socket.
        static
        std::unique_ptr<Socket>
        create(Protocol protocol,
               const std::string& hostname,
               int port,
               DurationOpt connection_timeout);
      protected:
        void
        _pacify_streambuffer();

      /*------.
      | Close |
      `------*/
      public:
        /// Close the Socket.
        virtual
        void
        close() = 0;

      /*------.
      | Write |
      `------*/
      public:
        /// Write the given buffer to the Socket.
        ///
        /// @param buffer The payload to write.
        virtual
        void
        write(elle::ConstWeakBuffer buffer) = 0;

      /*-----.
      | Read |
      `-----*/
      public:
        /// Read data from the socket.
        ///
        /// @param buffer The destination buffer.
        /// @param timeout The maximum duration before reading times out because
        ///                no data is available.
        /// @param bytes_read If not null, filled with the number of bytes read.
        virtual
        void
        read(elle::WeakBuffer buffer,
             DurationOpt timeout = DurationOpt(),
             int* bytes_read = nullptr);
        /// Read some data and return the number of bytes read.
        ///
        /// @param buffer The Destination buffer.
        /// @param timeout The maximum duration before reading times out because
        ///                no data is available.
        /// @param bytes_read If not null, filled with the number of bytes read.
        /// @returns The number of bytes read.
        virtual
        Size
        read_some(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
                  int* bytes_read = nullptr) = 0;
        /// Convenience wrapper around ::read returning an elle::Buffer.
        ///
        /// @param size The number of bytes to read.
        /// @param timeout The maximum duration before reading times out because
        ///                not enough data is available.
        /// @returns An elle::Buffer containing the data.
        elle::Buffer
        read(Size size, DurationOpt timeout = DurationOpt());
        /// Convenience wrapper around ::read_some returning an elle::Buffer.
        ///
        /// @param size The maximum number of byte to read.
        /// @param timeout The maximum duration before reading times out because
        ///                no data is available.
        /// @returns An elle::Buffer containing the data.
        elle::Buffer
        read_some(Size size, DurationOpt timeout = DurationOpt());
        /// Read until the delimiter is found.
        ///
        /// Delimiter is included in the resulting Buffer.
        ///
        /// @param delimiter The delimiter.
        /// @param timeout The maximum duration before reading times out because
        ///                the delimiter wasn't found.
        /// @returns An elle::Buffer containing the data.
        virtual
        elle::Buffer
        read_until(std::string const& delimiter,
                   DurationOpt opt = DurationOpt()) = 0;

     /*----------------.
     | Pretty printing |
     `----------------*/
      public:
        virtual
        void
        print(std::ostream& s) const = 0;
      };

      /// Abstract wrapper around an AsioSocket, offering an minimal interface
      /// to bind the underlying AsioSocket.
      ///
      /// Binding the AsioSocket is done by _connect, called by one of the
      /// PlainSocket constructors.
      template <typename AsioSocket_,
                typename EndPoint_ = typename AsioSocket_::endpoint_type>
      class PlainSocket
        : public Socket
        , public elle::Printable
      {
      /*--------.
      | Types.  |
      `--------*/
      public:
        /// Self type.
        using Self = PlainSocket<AsioSocket_, EndPoint_>;
        /// Super type.
        using Super = elle::reactor::network::Socket;
        /// Underlying asio socket type.
        using AsioSocket = AsioSocket_;
        /// End point type for the asio socket type.
        using EndPoint = EndPoint_;

        using SocketPtr =
          std::unique_ptr<AsioSocket, std::function<void (AsioSocket*)>>;

      /*-------------.
      | Construction |
      `-------------*/
      protected:
        /// Create an unbound PlainSocket, taking the ownership of a given
        /// AsioSocket.
        ///
        /// @param socket A Socket.
        PlainSocket(SocketPtr socket);
        /// Create and connect a PlainSocket.
        ///
        /// @param socket A Socket.
        /// @param peer The EndPoint of the peer.
        /// @param timeout The maximum duration before the connection attempt
        ///                times out.
        PlainSocket(std::unique_ptr<AsioSocket> socket,
                    EndPoint const& peer,
                    DurationOpt timeout);
        /// Create an unbound PlainSocket, taking the ownership of a given
        /// AsioSocket and storing peer.
        ///
        /// _connect is not called.
        ///
        /// @param An AsioSocket.
        /// @param The EndPoint of the peer.
        PlainSocket(std::unique_ptr<AsioSocket> socket,
                    EndPoint const& peer);
        /// Move socket
        PlainSocket(Self&& src);
        /// Destroy a socket.
        ~PlainSocket() override;

      /*-----------.
      | Connection |
      `-----------*/
      public:
        /// Close the underlying AsioSocket.
        void
        close() override;
      private:
        /// Perform the connection, according to the AsioSocket type.
        ///
        /// @param peer The EndPoint of the peer.
        /// @param timeout The maximum duration before the connection attempt
        ///                times out.
        void
        _connect(EndPoint const& peer, DurationOpt timeout);
        /// Disconnect the underlying AsioSocket.
        void
        _disconnect();

      /*-----------.
      | Properties |
      `-----------*/
      public:
        /// Return the EndPoint of the peer.
        virtual
        EndPoint
        peer() const;

        /// Return out local EndPoint.
        virtual
        EndPoint
        local_endpoint() const;

      /*----------------.
      | Pretty printing |
      `----------------*/
      public:
        void
        print(std::ostream& s) const override;

      /*------------.
      | Asio socket |
      `------------*/
      protected:
        friend class FingerprintedSocket;
        friend class SSLSocket;
        friend class SSLServer;
        friend class TCPServer;
        friend class TCPSocket;
        template <typename AsioSocket>
        friend class SocketOperation;
        ELLE_ATTRIBUTE_R(SocketPtr, socket);
        EndPoint _peer;
      };

      /// A specialization of PlainSocket supporting a stream API.
      template <typename AsioSocket,
                typename EndPoint = typename AsioSocket::endpoint_type>
      class StreamSocket
        : public PlainSocket<AsioSocket, EndPoint>
      {
      /*------.
      | Types |
      `------*/
      public:
        /// Self type.
        using Self = StreamSocket;
        /// Super type.
        using Super = PlainSocket<AsioSocket, EndPoint>;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        using Super::Super;
        StreamSocket(Self&& socket)
          : Super(std::move(socket))
        {}

        StreamSocket(AsioSocket* socket)
          : Super(std::unique_ptr<
              AsioSocket, std::function<void (AsioSocket*)>
            >(socket, [] (AsioSocket*) {} ))
        {}

        ~StreamSocket() override;

      public:
        using Super::read;
        /// @see Socket::read.
        ///
        /// This actually call Self::_read.
        void
        read(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
             int* bytes_read = nullptr) override;
        using Super::read_some;
        /// @see Socket::read_some.
        ///
        /// This actually call Self::_read.
        Size
        read_some(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
                  int* bytes_read = nullptr) override;
        /// @see Socket::read_until.
        elle::Buffer
        read_until(std::string const& delimiter,
                   DurationOpt opt = DurationOpt()) override;

      private:
        /// Read data from the Socket.
        ///
        /// @param buffer The destination buffer.
        /// @param timeout The maximum duration before reading times out because
        ///                no data is available.
        /// @param some Whether we are performing a read_some or a read.
        /// @param bytes_read If not null, filled with the number of bytes read.
        /// @returns The number of bytes read.
        virtual
        Size
        _read(elle::WeakBuffer buffer,
              DurationOpt timeout,
              bool some,
              int* bytes_read = nullptr);

        ELLE_ATTRIBUTE(boost::asio::streambuf, streambuffer);

      /*------.
      | Write |
      `------*/
      public:
        /// @Socket::write.
        void
        write(elle::ConstWeakBuffer buffer) override;
      protected:
        void
        _final_flush();
      private:
        void
        _async_write();
        ELLE_ATTRIBUTE(Mutex, write_mutex);
        ELLE_ATTRIBUTE(std::list<elle::Buffer>, async_writes);

      /*-----------------.
      | Concrete sockets |
      `-----------------*/
      protected:
        friend class TCPServer;
        friend class TCPSocket;
        // friend class UDPServer;
        // friend class UDPSocket;
      };
    }
  }
}
