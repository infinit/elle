#ifndef INFINIT_REACTOR_NETWORK_SOCKET_OPERATION_HH
# define INFINIT_REACTOR_NETWORK_SOCKET_OPERATION_HH

# include <boost/asio.hpp>

# include <reactor/network/exception.hh>
# include <reactor/network/socket.hh>
# include <reactor/operation.hh>

namespace reactor
{
  namespace network
  {
    template <typename AsioSocket>
    class SocketOperation: public Operation
    {
    public:
      SocketOperation(Scheduler& scheduler,
                      AsioSocket& socket);
    protected:
      virtual
      void
      _abort();
      void
      _wakeup(const boost::system::error_code& error);
      virtual
      void
      _handle_error(boost::system::error_code const& error);
      ELLE_ATTRIBUTE_R(AsioSocket&, socket);
      ELLE_ATTRIBUTE_R(bool, canceled);
    };
  }
}

#endif
