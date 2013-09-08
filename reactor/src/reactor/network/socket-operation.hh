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
                        PlainSocket<AsioSocket>* socket):
          Operation(scheduler),
          _socket(socket),
          _canceled(new bool(false))
        {}

      protected:
        AsioSocket*
        socket()
        {
          return _socket->_socket;
        }

        AsioSocket const*
        socket() const
        {
          return _socket->_socket;
        }

        virtual void _abort()
        {
          try
          {
            *this->_canceled = true;
            socket()->cancel();
          }
          catch (boost::system::system_error const&)
          {
            // Cancel may fail if for instance the socket was closed
            // manually. If cancel fails, assume the operation is de facto
            // cancelled and we can carry on. I no of no case were we "were not
            // actually able to cancel the operation".
          }
          _signal();
        }

      protected:
        void _wakeup(std::shared_ptr<bool> canceled,
                     const boost::system::error_code& error)
        {
          if (*canceled)
            return;
          if (error)
            this->_raise<Exception>(error.message());
          _signal();
        }
        PlainSocket<AsioSocket>* _socket;
        std::shared_ptr<bool> _canceled;
    };
  }
}

#endif
