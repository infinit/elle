#include <elle/log.hh>

#include <reactor/network/SocketOperation.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.SocketOperation");

namespace reactor
{
  namespace network
  {
    template <typename AsioSocket>
    SocketOperation<AsioSocket>::SocketOperation(
      Scheduler& scheduler,
      AsioSocket& socket):
      Operation(scheduler),
      _socket(socket),
      _canceled(new bool(false))
    {}

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_abort()
    {
      ELLE_TRACE_SCOPE("%s: abort", *this);
      *this->_canceled = true;
      boost::system::error_code ec;
      this->_socket.cancel(ec);
      // Cancel may fail if for instance the socket was closed manually. If
      // cancel fails, assume the operation is de facto cancelled and we can
      // carry on. I know of no case were we "were not actually able to
      // cancel the operation".
      (void) ec;
      reactor::wait(*this);
    }

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_wakeup(
      std::shared_ptr<bool> canceled,
      const boost::system::error_code& error)
    {
      if (*canceled)
      {
        ELLE_TRACE_SCOPE("%s: ended: cancelled", *this);
        this->_signal();
        return;
      }
      if (error)
      {
        ELLE_TRACE_SCOPE("%s: ended with error: %s", *this, error.message());
        this->_raise<Exception>(error.message());
        this->_signal();
        return;
      }
      ELLE_TRACE_SCOPE("%s: ended", *this);
      this->_signal();
    }

    template class SocketOperation<boost::asio::ip::tcp::socket>;
    template class SocketOperation<boost::asio::ip::udp::socket>;
  }
}
