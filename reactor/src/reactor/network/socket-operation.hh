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
                        PlainSocket<AsioSocket>* socket);
      protected:
        AsioSocket*
        socket();
        AsioSocket const*
        socket() const;
        virtual
        void
        _abort();
        void
        _wakeup(std::shared_ptr<bool> canceled,
                const boost::system::error_code& error);
        PlainSocket<AsioSocket>* _socket;
        std::shared_ptr<bool> _canceled;
    };
  }
}

#endif
