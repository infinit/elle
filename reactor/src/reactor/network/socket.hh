#ifndef INFINIT_REACTOR_NETWORK_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_SOCKET_HH

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/attribute.hh>

# include <reactor/asio.hh>
# include <reactor/duration.hh>
# include <reactor/mutex.hh>
# include <reactor/network/fwd.hh>
# include <reactor/network/Protocol.hh>

namespace reactor
{
  namespace network
  {
    template <typename AsioSocket>
    class SocketOperation;

    class Socket:
      public elle::IOStream
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
      virtual
      ~Socket();
      /** Create a socket for the given protocol.
       *  @param protocol The transport protocl to use.
       */
      static
      std::unique_ptr<Socket>
      create(Protocol protocol,
             const std::string& hostname,
             int port,
             DurationOpt connection_timeout);
      void
      _pacify_streambuffer();

    /*------.
    | Close |
    `------*/
    public:
      virtual
      void
      close() = 0;

    /*------.
    | Write |
    `------*/
    public:
      virtual
      void
      write(elle::ConstWeakBuffer buffer) = 0;

    /*-----.
    | Read |
    `-----*/
    public:
      virtual
      void
      read(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
           int* bytes_read = nullptr);
      virtual
      Size
      read_some(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
                int* bytes_read = nullptr) = 0;
      elle::Buffer
      read(Size size, DurationOpt timeout = DurationOpt());
      elle::Buffer
      read_some(Size size, DurationOpt timeout = DurationOpt());
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

    template <typename AsioSocket_,
              typename EndPoint_ = typename AsioSocket_::endpoint_type>
    class PlainSocket
      : public Socket
      , public elle::Printable
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      /// Self type.
      typedef PlainSocket<AsioSocket_, EndPoint_> Self;
      /// Super type.
      typedef Socket Super;
      /// Underlying asio socket type.
      typedef AsioSocket_ AsioSocket;
      /// End point type for the asio socket type.
      typedef EndPoint_ EndPoint;

    /*-------------.
    | Construction |
    `-------------*/
    protected:
      /// Create an unbound socket.
      PlainSocket(std::unique_ptr<AsioSocket> socket);
      /// Create and connect socket.
      PlainSocket(std::unique_ptr<AsioSocket> socket,
                  EndPoint const& peer,
                  DurationOpt timeout);
      /// Create wrapping socket.
      PlainSocket(std::unique_ptr<AsioSocket> socket,
                  EndPoint const& peer);
      /// Move socket
      PlainSocket(Self&& src);
      /// Destroy a socket.
      virtual
      ~PlainSocket();

    /*-----------.
    | Connection |
    `-----------*/
    public:
      virtual
      void
      close() override;
    private:
      void
      _connect(EndPoint const& peer, DurationOpt timeout);
      void
      _disconnect();

    /*-----------.
    | Properties |
    `-----------*/
    public:
      virtual
      EndPoint
      peer() const;

      virtual
      EndPoint
      local_endpoint() const;

    /*----------------.
    | Pretty printing |
    `----------------*/
    public:
      virtual
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
      ELLE_ATTRIBUTE_R(std::unique_ptr<AsioSocket>, socket);
      EndPoint _peer;
    };

    template <typename AsioSocket,
              typename EndPoint = typename AsioSocket::endpoint_type>
    class StreamSocket
      : public PlainSocket<AsioSocket, EndPoint>
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      /// Self type.
      typedef StreamSocket<AsioSocket, EndPoint> Self;
      /// Super type.
      typedef PlainSocket<AsioSocket, EndPoint> Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      // XXX: gcc 4.7 can't use parent's constructor.
      StreamSocket(std::unique_ptr<AsioSocket> socket,
                   EndPoint const& peer,
                   DurationOpt timeout):
        Super(std::move(socket), peer, timeout)
      {}

      // XXX: gcc 4.7 can't use parent's constructor.
      StreamSocket(std::unique_ptr<AsioSocket> socket,
                   EndPoint const& peer):
        Super(std::move(socket), peer)
      {}

      StreamSocket(Self&& socket)
        : Super(std::move(socket))
      {}

      virtual
      ~StreamSocket();

    /*-----.
    | Read |
    `-----*/
    public:
      using Super::read;
      virtual
      void
      read(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
           int* bytes_read = nullptr) override;
      using Super::read_some;
      virtual
      Size
      read_some(elle::WeakBuffer buffer, DurationOpt timeout = DurationOpt(),
                int* bytes_read = nullptr) override;

      elle::Buffer
      read_until(std::string const& delimiter,
                 DurationOpt opt = DurationOpt()) override;

    private:
      virtual
      Size
      _read(elle::WeakBuffer buffer, DurationOpt timeout,
            bool some, int* bytes_read=nullptr);

      ELLE_ATTRIBUTE(boost::asio::streambuf, streambuffer);

    /*------.
    | Write |
    `------*/
    public:
      virtual
      void
      write(elle::ConstWeakBuffer buffer) override;
    protected:
      void
      _final_flush();
    private:
      ELLE_ATTRIBUTE(Mutex, write_mutex);

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

#endif
