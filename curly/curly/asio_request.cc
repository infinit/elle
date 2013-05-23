#include <curly/asio_request.hh>

namespace asio = boost::asio;

namespace curly
{
  size_t
  asio_request::write_helper(char* data,
                        size_t size, size_t nmemb,
                        void* userptr)
  {
    auto* _this = reinterpret_cast<asio_request*>(userptr);
    (*_this->_config._output) << std::string{data, data + (size * nmemb)};
    return size * nmemb;
  }

  size_t
  asio_request::read_helper(char* data,
                            size_t size, size_t nmemb,
                            void* userptr)
  {
    auto* _this = reinterpret_cast<asio_request*>(userptr);
    _this->_config._input->read(data, size * nmemb);
    return _this->_config._input->gcount();
  }

  int
  asio_request::open_socket(void *userptr,
                            curlsocktype purpose,
                            struct curl_sockaddr *address)
  {
    auto& self = *reinterpret_cast<asio_request*>(userptr);
    curl_socket_t sockfd = CURL_SOCKET_BAD;

    /* restrict to ipv4 */
    if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
    {
      sockfd = self._socket.native_handle();
    }
    return sockfd;
  }

  int
  asio_request::close_socket(void *userptr,
               curl_socket_t sockfd)
  {
    (void)userptr;
    (void)sockfd;
    //no much to do
    return 0;
  }

  asio_request::asio_request(asio::io_service& io,
                             curly::request_configuration c):
    _io(asio::use_service<curl_service>(io)),
    _config{c},
    _socket{io, boost::asio::ip::tcp::v4()}
  {
    _config.option(CURLOPT_OPENSOCKETFUNCTION, &asio_request::open_socket);
    _config.option(CURLOPT_OPENSOCKETDATA, this);
    _config.option(CURLOPT_PRIVATE, this);

    _config.option(CURLOPT_CLOSESOCKETFUNCTION, &asio_request::close_socket);
    _config.option(CURLOPT_CLOSESOCKETDATA, this);
    if (this->_config._output)
    {
      _config.option(CURLOPT_WRITEFUNCTION, &asio_request::write_helper);
      _config.option(CURLOPT_WRITEDATA, this);
    }
    if (this->_config._input != nullptr)
    {
      _config.option(CURLOPT_READFUNCTION, &asio_request::read_helper);
      _config.option(CURLOPT_READDATA, this);
    }
  }

  void
  asio_request::async_perform(completion_callback const& f)
  {
    this->_callback = f;
    _io.add(this);
  }
} /* curly */
