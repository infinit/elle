#ifndef INFINIT_REACTOR_NETWORK_SOCKET_OPERATION_HH
# define INFINIT_REACTOR_NETWORK_SOCKET_OPERATION_HH

# include <boost/asio.hpp>

# include <reactor/network/exception.hh>
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
                        PlainSocket<AsioSocket>* socket)
          : Operation(scheduler)
          , _socket(socket)
        {}

      protected:
        AsioSocket* socket()
        {
          return _socket->_socket;
        }

        virtual void _abort()
        {
          socket()->cancel();
          _signal();
        }

      protected:
        void _wakeup(const boost::system::error_code& error)
        {
          if (error)
            _raise(new Exception(scheduler(), error.message()));
          _signal();
        }
        PlainSocket<AsioSocket>* _socket;
    };
  }
}

#endif
