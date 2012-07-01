#ifndef ELLE_NETWORK_TCPSOCKET_HH
# define ELLE_NETWORK_TCPSOCKET_HH

# include <elle/types.hh>

# include <elle/concurrency/Event.hh>

# include <elle/network/fwd.hh>

# include <reactor/network/tcp-socket.hh>
# include <reactor/mutex.hh>
# include <reactor/thread.hh>

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
      TCPSocket(reactor::network::Socket* socket);
      ~TCPSocket();

    /*--------.
    | Sending |
    `--------*/
    public:
      void Write(const Packet& packet);

    /*----------.
    | Reception |
    `----------*/
    public:
      Parcel* Read();
    private:
      unsigned char*    _buffer;
      int               _buffer_size;
      int               _buffer_capacity;

      //
    public:
      void Disconnect();

      template <typename I>
      Status            Send(const I,
                             const concurrency::Event& =
                               concurrency::Event::Null);
      template <typename O>
      Status            Receive(concurrency::Event&, O);
      template <typename I,
                typename O>
      Status            Call(const I,
                             O);
      template <typename I>
      Status            Reply(const I);
    /*--------.
    | Details |
    `--------*/

    private:
      void Dispatch();
      reactor::network::Socket* _socket;
      reactor::Mutex _socket_write_lock;
      reactor::Thread* _dispatcher;
    };

    class Context
    {
    public:
      Parcel* parcel;
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
