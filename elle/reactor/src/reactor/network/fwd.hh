#ifndef INFINIT_REACTOR_NETWORK_FWD_HH
# define INFINIT_REACTOR_NETWORK_FWD_HH

# include <reactor/fwd.hh>

namespace reactor
{
  namespace network
  {
    class Buffer;
    class Socket;
    template <typename AsioSocket>
    class PlainSocket;
    class TCPSocket;
    class UDPSocket;
    class UDPServerSocket;
    class Server;
    class UDPServer;
    class TCPServer;

    /// A byte travelling on the network.
    typedef unsigned char Byte;
    /// A number of bytes.
    typedef unsigned int Size;

  }
}

#endif
