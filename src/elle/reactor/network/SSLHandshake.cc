#include <elle/reactor/network/SSLHandshake.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      template <typename Socket>
      SSLHandshake<Socket>::SSLHandshake(Socket& stream,
                                         typename Socket::handshake_type type)
        : SocketOperation(stream.next_layer())
        , _stream(stream)
        , _type(type)
      {}

      template <typename Socket>
      void
      SSLHandshake<Socket>::print(std::ostream& stream) const
      {
        elle::print(stream, "SSL handshake {}", this->_stream);
      }

      template <typename Socket>
      void
      SSLHandshake<Socket>::_start()
      {
        this->_stream.async_handshake(
          this->_type,
          [this](const boost::system::error_code& error)
          {
            this->_wakeup(error);
          });
      }

      template <typename Socket>
      void
      SSLHandshake<Socket>::_handle_error(
        boost::system::error_code const& error)
      {
        this->_raise<SSLHandshakeError>(error.message());
      }
    }
  }
}
