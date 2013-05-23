#include <curly/curly_service.hh>
#include <curly/asio_request.hh>

namespace asio = boost::asio;

static void
_throw_if_mcode(std::string const& where,
                int line,
                CURLMcode code)
{
  if (code != CURLM_OK)
  {
    std::stringstream ss;
    std::string msg;
    switch (code)
    {
      case CURLM_CALL_MULTI_PERFORM: msg = "CURLM_CALL_MULTI_PERFORM";  break;
      case CURLM_BAD_HANDLE:         msg = "CURLM_BAD_HANDLE";          break;
      case CURLM_BAD_EASY_HANDLE:    msg = "CURLM_BAD_EASY_HANDLE";     break;
      case CURLM_OUT_OF_MEMORY:      msg = "CURLM_OUT_OF_MEMORY";       break;
      case CURLM_INTERNAL_ERROR:     msg = "CURLM_INTERNAL_ERROR";      break;
      case CURLM_UNKNOWN_OPTION:     msg = "CURLM_UNKNOWN_OPTION";      break;
      case CURLM_LAST:               msg = "CURLM_LAST";                break;
      case CURLM_BAD_SOCKET:         msg = "CURLM_BAD_SOCKET";          break;
      default:                       msg = "CURLM_unknown";             break;
    }
    ss
      << "error code: " << code 
      << " in " << where << ":" << line
      << ": " << msg;
    throw std::runtime_error{ss.str()};
  }
}

namespace curly
{
  asio::io_service::id curl_service::id;

  void
  curl_multi_deleter::operator ()(CURLM* ptr)
  {
    auto mc = curl_multi_cleanup(ptr);
    throw_if_mcode(mc);
  }

  curl_service::curl_service(asio::io_service& io):
    asio::io_service::service(io),
    _multi{curl_multi_init()},
    _timer{io}
  {
    this->option(CURLMOPT_TIMERFUNCTION, &curl_service::timeout_helper);
    this->option(CURLMOPT_TIMERDATA, this);
  
    this->option(CURLMOPT_SOCKETFUNCTION, &curl_service::poll_helper);
    this->option(CURLMOPT_SOCKETDATA, this);
  }

  int
  curl_service::timeout_helper(CURLM *multi,
                               long timeout_ms,
                               void *userptr)
  {
    auto* _this = reinterpret_cast<curl_service*>(userptr);
  
    /* cancel running timer */
    _this->_timer.cancel();
  
    auto timeout_fn = [multi, userptr, _this]
      (boost::system::error_code const& error)
    {
      if (!error)
      {
        auto mc = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                           &_this->_concurrent);
        throw_if_mcode(mc);
        _this->check_multi_info();
      }
    };
    if (timeout_ms > 0)
    {
      /* update timer */
      _this->_timer.expires_from_now(boost::posix_time::millisec(timeout_ms));
      _this->_timer.async_wait(timeout_fn);
    }
    else
    {
      /* call timeout function immediately */
      boost::system::error_code error; /*success*/
      timeout_fn(error);
    }
  
    return 0;
  }

  int
  curl_service::poll_helper(CURL *easy_handle,
                            curl_socket_t sockfd,
                            int what,
                            void *userptr,
                            void *socket_userptr)
  {
    (void)easy_handle;
    auto* _this = reinterpret_cast<curl_service*>(userptr);
    auto* req = reinterpret_cast<asio_request*>(socket_userptr);

    // We can not assign a per socket userptr until the socket have been
    // properly registered into the multi_handle.
    // This function is the very first place where it's correct to do that.
    if (req == nullptr)
    {
      for (auto* request: _this->_requests)
      {
        if (request->_socket.native_handle() == sockfd)
        {
          auto mc = curl_multi_assign(_this->_multi.get(), sockfd, request);
          throw_if_mcode(mc);
          req = request;
          break;
        }
      }
    }

    // The generic callback for read/write
    // 'what' describe the action to perform.
    auto fn = [_this, what, sockfd]
      (boost::system::error_code const&, size_t)
    {
      auto mc = curl_multi_socket_action(_this->_multi.get(), sockfd,
                                         what, &_this->_concurrent);
      throw_if_mcode(mc);
      _this->check_multi_info();
    };

    // Check what we have to poll for. The asio null_buffers allow checking only
    // for readiness.
    switch (what)
    {
      case CURL_POLL_IN:
        req->_socket.async_read_some(asio::null_buffers(), fn);
        break;
      case CURL_POLL_OUT:
        req->_socket.async_write_some(asio::null_buffers(), fn);
        break;
      case CURL_POLL_INOUT:
        req->_socket.async_read_some(asio::null_buffers(), fn);
        req->_socket.async_write_some(asio::null_buffers(), fn);
        break;
      case CURL_POLL_REMOVE:
        break;
      default:
        break;
    }
    return  0;
  }

  void
  curl_service::check_multi_info()
  {
    CURLMsg *msg;
    int msgs_left;
    CURL *easy;
    CURLcode res;
    asio_request *req;

    while ((msg = curl_multi_info_read(this->_multi.get(), &msgs_left)))
    {
      // at this point, there is only on type of info:
      if (msg->msg == CURLMSG_DONE)
      {
        easy = msg->easy_handle;
        res = msg->data.result;
        curl_easy_getinfo(easy, CURLINFO_PRIVATE, &req);
        req->_callback(res);
      }
    }
  }


  void
  curl_service::add(asio_request* ptr)
  {
    auto mc = curl_multi_add_handle(this->_multi.get(),
                                    ptr->_config.native_handle());
    throw_if_mcode(mc);
    this->_requests.insert(ptr);
  }
  
  void
  curl_service::shutdown_service()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
} /* curly */
