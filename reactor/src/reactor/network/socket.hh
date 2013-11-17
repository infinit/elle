#ifndef INFINIT_REACTOR_NETWORK_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_SOCKET_HH

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/attribute.hh>
# include <elle/network/Locus.hh>

# include <reactor/duration.hh>
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

     /*------.
     | Locus |
     `------*/
      public:
        virtual elle::network::Locus local_locus() const = 0;
        virtual elle::network::Locus remote_locus() const = 0;

     /*----------------.
     | Pretty printing |
     `----------------*/
      public:
        virtual void print(std::ostream& s) const = 0;
    };
    std::ostream& operator << (std::ostream& s, const Socket& socket);

    template <typename AsioSocket_>
    class PlainSocket: public Socket
    {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        /// Self type.
        typedef Socket Self;
        /// Super type.
        typedef Socket Super;
        /// Underlying asio socket type.
        typedef AsioSocket_ AsioSocket;
        /// End point type for the asio socket type.
        typedef typename AsioSocket::endpoint_type EndPoint;

    /*-------------.
    | Construction |
    `-------------*/
    protected:
      /// Create and connect socket.
      PlainSocket(Scheduler& sched,
                  EndPoint const& peer,
                  DurationOpt timeout = DurationOpt());
      /// Create wrapping socket.
      PlainSocket(Scheduler& sched, AsioSocket* socket);
      /// Destroy a socket.
      virtual ~PlainSocket();

    /*-----------.
    | Connection |
    `-----------*/
    protected:
      void _connect(const EndPoint& peer, DurationOpt timeout = DurationOpt());
      void _disconnect();

    /*-----------.
    | Properties |
    `-----------*/
    public:
      virtual elle::network::Locus local_locus() const;
      virtual elle::network::Locus remote_locus() const;
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
      ELLE_ATTRIBUTE_R(AsioSocket*, socket);
      EndPoint _peer;
    };
  }
}

#endif
