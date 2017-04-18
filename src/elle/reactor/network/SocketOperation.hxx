
namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*----------------.
      | SocketOperation |
      `----------------*/

      template <typename AsioSocket>
      SocketOperation<AsioSocket>::SocketOperation(
        AsioSocket& socket)
        : Operation(*elle::reactor::Scheduler::scheduler())
        , _socket(socket)
        , _canceled(false)
      {}

      template <typename AsioSocket>
      void
      SocketOperation<AsioSocket>::_abort()
      {
        this->_canceled = true;
        boost::system::error_code ec;
        this->_socket.cancel(ec);
        // Cancel may fail if for instance the socket was closed manually. If
        // cancel fails, assume the operation is de facto cancelled and we can
        // carry on. I know of no case were we "were not actually able to
        // cancel the operation".
        (void) ec;
        elle::reactor::wait(*this);
      }

      template <typename AsioSocket>
      void
      SocketOperation<AsioSocket>::_wakeup(
        const boost::system::error_code& error)
      {
        ELLE_LOG_COMPONENT("elle.reactor.network.SocketOperation");
        if (this->_canceled)
          ELLE_TRACE_SCOPE("%s: ended: cancelled", *this);
        else if (error)
        {
          ELLE_TRACE_SCOPE("%s: ended with error: %s (%s)", *this, error.message(), error);
          this->_handle_error(error);
        }
        else
          ELLE_TRACE_SCOPE("%s: ended", *this);
        this->done();
      }

      template <typename AsioSocket>
      void
      SocketOperation<AsioSocket>::_handle_error(
        boost::system::error_code const& error)
      {
        if (error == boost::asio::error::connection_refused
            || error.value() == EADDRNOTAVAIL)
          this->_raise<ConnectionRefused>();
        else if (error == boost::asio::error::eof)
          this->_raise<ConnectionClosed>();
        else
        {
          ELLE_ASSERT_NEQ(error, boost::system::errc::operation_canceled);
          this->_raise<Error>(error.message());
        }
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
}
