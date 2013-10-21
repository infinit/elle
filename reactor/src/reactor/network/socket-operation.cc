#include <reactor/network/socket-operation.hh>

namespace reactor
{
  namespace network
  {
    template <typename AsioSocket>
    SocketOperation<AsioSocket>::SocketOperation(
      Scheduler& scheduler,
      PlainSocket<AsioSocket>* socket):
      Operation(scheduler),
      _socket(socket),
      _canceled(new bool(false))
    {}

    template <typename AsioSocket>
    AsioSocket*
    SocketOperation<AsioSocket>::socket()
    {
      return _socket->_socket;
    }

    template <typename AsioSocket>
    AsioSocket const*
    SocketOperation<AsioSocket>::socket() const
    {
      return _socket->_socket;
    }

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_abort()
    {
      *this->_canceled = true;
      boost::system::error_code ec;
      socket()->cancel(ec);

      // Cancel may fail if for instance the socket was closed manually. If
      // cancel fails, assume the operation is de facto cancelled and we can
      // carry on. I know of no case were we "were not actually able to
      // cancel the operation".
      (void) ec;

      _signal();
    }

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_wakeup(
      std::shared_ptr<bool> canceled,
      const boost::system::error_code& error)
    {
      if (*canceled)
        return;
      if (error)
        this->_raise<Exception>(error.message());
      _signal();
    }

    template class SocketOperation<boost::asio::ip::tcp::socket>;
    template class SocketOperation<boost::asio::ip::udp::socket>;
  }
}
