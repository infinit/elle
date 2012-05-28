#ifndef ELLE_NETWORK_TCPSOCKET_HH
# define ELLE_NETWORK_TCPSOCKET_HH

# include <elle/concurrency/Event.hh>
# include <elle/concurrency/Signal.hh>
# include <elle/core/Natural.hh>
# include <elle/core/String.hh>
# include <elle/fwd.hh>
# include <elle/network/Packet.hh>
# include <elle/network/Parcel.hh>
# include <elle/network/Port.hh>
# include <elle/network/Session.hh>
# include <elle/network/fwd.hh>
# include <elle/radix/Status.hh>
# include <elle/standalone/Region.hh>
# include <reactor/network/tcp-socket.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

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
      void ReadData();
      unsigned char*    _buffer;
      int               _buffer_size;
      int               _buffer_capacity;

      //
    public:
      void Disconnect();

      template <typename I>
      Status            Send(const I,
                             const Event& = Event::Null);
      template <typename O>
      Status            Receive(Event&, O);
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
      reactor::Thread* _dispatcher;
    };

    class Context
    {
    public:
      Parcel* parcel;
      TCPSocket* socket;
      std::string host;
    };

    Context current_context();
    void current_context(Context socket);
  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/TCPSocket.hxx>

#endif
