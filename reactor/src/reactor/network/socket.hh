#ifndef INFINIT_REACTOR_NETWORK_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_SOCKET_HH

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/attribute.hh>
# include <elle/network/Locus.hh>

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

    class Socket: public elle::IOStream
    {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        /// Self type.
        typedef Socket Self;

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
        Socket(Scheduler& sched);
        /// Destroy a socket.
        virtual ~Socket();
        /** Create a socket for the given protocol.
         *  @param protocol The transport protocl to use.
         *  @param sched The underlying scheduler.
         */
        static
        std::unique_ptr<Socket>
        create(Protocol protocol,
               Scheduler& sched,
               const std::string& hostname,
               int port,
               DurationOpt connection_timeout);
      protected:
        void
        _pacify_streambuffer();

      /*------.
      | Write |
      `------*/
      public:
        virtual void write(elle::ConstWeakBuffer buffer) = 0;

      /*-----.
      | Read |
      `-----*/
      public:
        virtual void read(Buffer buffer,
                          DurationOpt timeout = DurationOpt());
        virtual Size read_some(Buffer buffer,
                               DurationOpt timeout = DurationOpt()) = 0;

      /*-----------.
      | Scheduling |
      `-----------*/
      public:
        Scheduler& scheduler();
      private:
        Scheduler& _sched;

     /*----------------.
     | Pretty printing |
     `----------------*/
      public:
        virtual void print(std::ostream& s) const = 0;
    };
    std::ostream& operator << (std::ostream& s, const Socket& socket);

    template <typename AsioSocket_,
              typename EndPoint_ = typename AsioSocket_::endpoint_type>
    class PlainSocket: public Socket
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
      /// Create and connect socket.
      PlainSocket(Scheduler& sched,
                  std::unique_ptr<AsioSocket> socket,
                  EndPoint const& peer,
                  DurationOpt timeout);
      /// Create wrapping socket.
      PlainSocket(Scheduler& sched,
                  std::unique_ptr<AsioSocket> socket,
                  EndPoint const& peer);
      /// Destroy a socket.
      virtual ~PlainSocket();

    /*-----------.
    | Connection |
    `-----------*/
    public:
      void close();
    private:
      void
      _connect(EndPoint const& peer, DurationOpt timeout);
      void
      _disconnect();

    /*-----------.
    | Properties |
    `-----------*/
    public:
      EndPoint peer() const;
      EndPoint local_endpoint() const;

    /*----------------.
    | Pretty printing |
    `----------------*/
    public:
      virtual void print(std::ostream& s) const;

    /*------------.
    | Asio socket |
    `------------*/
    protected:
      friend class TCPServer;
      friend class TCPSocket;
      friend class UDPServer;
      friend class UDPSocket;
      template <typename AsioSocket>
      friend class SocketOperation;
      ELLE_ATTRIBUTE_R(std::unique_ptr<AsioSocket>, socket);
      EndPoint _peer;
    };

    template <typename AsioSocket_,
              typename EndPoint_ = typename AsioSocket_::endpoint_type>
    class StreamSocket:
      public PlainSocket<AsioSocket_, EndPoint_>
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      /// Self type.
      typedef StreamSocket<AsioSocket_, EndPoint_> Self;
      /// Super type.
      typedef PlainSocket<AsioSocket_, EndPoint_> Super;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      using Super::Super;
      virtual
      ~StreamSocket();

    /*-----.
    | Read |
    `-----*/
    public:
      virtual
      void
      read(Buffer buffer,
           DurationOpt timeout = DurationOpt());
      virtual
      Size
      read_some(Buffer buffer,
                DurationOpt timeout = DurationOpt());
      elle::Buffer
      read_until(std::string const& delimiter,
                 DurationOpt opt = DurationOpt());
    private:
      virtual Size _read(Buffer buffer,
                         DurationOpt timeout,
                         bool some);
      ELLE_ATTRIBUTE(boost::asio::streambuf, streambuffer);

    /*------.
    | Write |
    `------*/
    public:
      virtual void write(elle::ConstWeakBuffer buffer);
    private:
      Mutex _write_mutex;

    /*-----------------.
    | Concrete sockets |
    `-----------------*/
    protected:
      friend class TCPServer;
      friend class TCPSocket;
      friend class UDPServer;
      friend class UDPSocket;
    };
  }
}

#endif
