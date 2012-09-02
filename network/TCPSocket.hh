#ifndef ELLE_NETWORK_TCPSOCKET_HH
# define ELLE_NETWORK_TCPSOCKET_HH

# include <elle/network/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <reactor/network/tcp-socket.hh>
#  include <reactor/mutex.hh>
#  include <reactor/thread.hh>
# include <elle/idiom/Close.hh>

# include <protocol/fwd.hh>

namespace elle
{
  namespace network
  {
    ///
    /// this class represents a TCP socket which provides a reliable
    /// communication channel, contrary to UDPSocket.
    ///
    class TCPSocket
    {
    public:
      //
      // constants
      //
      static const Natural32            Timeout;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      TCPSocket(reactor::network::Socket* socket, bool master);
      ~TCPSocket();

    public:
      void Disconnect();

      template <typename I>
      void send(const I, infinit::protocol::Stream& channel);
      template <typename I>
      void send(const I);
      template <typename O>
      Status receive(infinit::protocol::Stream& channel, O);
      template <typename I,
                typename O>
      Status            Call(const I,
                             O);
      template <typename I>
      void reply(I const);

    /*--------.
    | Details |
    `--------*/

    private:
      void _dispatch_channels();
      void _dispatch_default_channel();
      void _dispatch_packet(infinit::protocol::Packet packet,
                            infinit::protocol::Stream& channel);
      Parcel* _read_parcel(infinit::protocol::Packet& packet);

      reactor::network::Socket* _socket;
      infinit::protocol::Stream* _stream;
      infinit::protocol::ChanneledStream* _channels;
      reactor::Mutex _stream_write_lock;
      reactor::Thread* _dispatcher_channels;
      reactor::Thread* _dispatcher_default;
    };

    class Context
    {
    public:
      //Parcel* parcel;
      infinit::protocol::Stream* channel;
      TCPSocket* socket;
      std::string host;
    };

    std::ostream&
    operator << (std::ostream& s, const TCPSocket& socket);

    Context current_context();
    void current_context(Context socket);
  }
}

#include <elle/network/TCPSocket.hxx>

#endif
