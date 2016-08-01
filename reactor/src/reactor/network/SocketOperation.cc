#include <boost/asio/ssl.hpp>

#include <elle/log.hh>

#include <reactor/network/SocketOperation.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.SocketOperation");

namespace reactor
{
  namespace network
  {
    /*----------------.
    | SocketOperation |
    `----------------*/

    template <typename AsioSocket>
    SocketOperation<AsioSocket>::SocketOperation(
      AsioSocket& socket):
      Operation(*reactor::Scheduler::scheduler()),
      _socket(socket),
      _canceled(false)
    {}

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_abort()
    {
      ELLE_TRACE_SCOPE("%s: abort", *this);
      this->_canceled = true;
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
      const boost::system::error_code& error)
    {
      if (this->_canceled)
        ELLE_TRACE_SCOPE("%s: ended: cancelled", *this);
      else if (error)
      {
        ELLE_TRACE_SCOPE("%s: ended with error: %s", *this, error.message());
        this->_handle_error(error);
      }
      else
        ELLE_TRACE_SCOPE("%s: ended", *this);
      this->done();
    }

    template <typename AsioSocket>
    void
    SocketOperation<AsioSocket>::_handle_error(
      const boost::system::error_code& error)
    {
      if (error == boost::asio::error::connection_refused)
        this->_raise<ConnectionRefused>();
      else
        this->_raise<Exception>(error.message());
    }

    template class SocketOperation<boost::asio::ip::tcp::socket>;
    template class SocketOperation<boost::asio::ip::udp::socket>;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
    template class SocketOperation<boost::asio::local::stream_protocol::socket>;
#endif

    /*--------------.
    | DataOperation |
    `--------------*/

    template <typename AsioSocket>
    DataOperation<AsioSocket>::DataOperation(AsioSocket& socket)
      : Super(socket)
    {}

    template <typename AsioSocket>
    void
    DataOperation<AsioSocket>::_handle_error(
      boost::system::error_code const& error)
    {
      if (error == boost::asio::error::eof ||
          error == boost::asio::error::broken_pipe ||
          error == boost::asio::error::connection_aborted ||
          error == boost::asio::error::connection_reset ||
          error == boost::asio::error::operation_aborted)
        this->template _raise<ConnectionClosed>();
      else if (error == boost::asio::error::bad_descriptor)
        this->template _raise<SocketClosed>();
      else if (error.category() == boost::asio::error::get_ssl_category() &&
               error.value() == ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ))
        this->template _raise<SSLShortRead>();
      else
        Super::_handle_error(error);
    }

    template class DataOperation<boost::asio::ip::tcp::socket>;
    template class DataOperation<boost::asio::ip::udp::socket>;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
    template class DataOperation<boost::asio::local::stream_protocol::socket>;
#endif
  }
}
