#pragma once

#include <elle/reactor/fwd.hh>

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
# define REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
#endif

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class Buffer;
      template <typename AsioSocket, typename EndPoint>
      class PlainSocket;
      class Server;
      class SSLServer;
      class SSLSocket;
      class Socket;
      class TCPServer;
      class TCPSocket;
      class UDPServer;
      class UDPServerSocket;
      class UDPSocket;
      class UTPSocket;
      class UTPServer;
# ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
      class UnixDomainServer;
      class UnixDomainSocket;
# endif
      /// A byte travelling on the network.
      typedef unsigned char Byte;
      /// A number of bytes.
      typedef unsigned int Size;

    }
  }
}
