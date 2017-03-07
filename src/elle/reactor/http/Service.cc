#include <curl/curl.h>

#include <elle/Exception.hh>
#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/http/RequestImpl.hh>
#include <elle/reactor/http/Service.hh>
#include <elle/reactor/http/exceptions.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.http.Service");

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      /*--------.
      | Helpers |
      `--------*/

      namespace
      {
        char const*
        action_string(int what)
        {
          switch (what)
          {
            case CURL_POLL_INOUT:
              return "read and write";
            case CURL_POLL_IN:
              return "read";
            case CURL_POLL_OUT:
              return "write";
            case CURL_POLL_REMOVE:
              return "be removed";
            case CURL_POLL_NONE:
              return "nothing";
          }
          elle::unreachable();
        }
      }

      /*-------------.
      | Construction |
      `-------------*/

      boost::asio::io_service::id Service::id;

      // Reference count to ensure that we only globally initialize and destroy
      // curl once.
      static
      int&
      curl_global_ref_count()
      {
        static int _curl_global_ref_count = 0;
        return _curl_global_ref_count;
      }

      Service::Service(boost::asio::io_service& service)
        : boost::asio::io_service::service(service)
        , _curl(nullptr)
        , _requests()
        , _timer(service)
      {
        if (curl_global_ref_count()++ == 0)
          if (curl_global_init(CURL_GLOBAL_DEFAULT))
            throw std::bad_alloc();
        this->_curl = curl_multi_init();
        if (!this->_curl)
          throw std::bad_alloc();
        curl_multi_setopt(this->_curl, CURLMOPT_SOCKETDATA, this);
        curl_multi_setopt(this->_curl, CURLMOPT_TIMERDATA, this);
        curl_multi_setopt(this->_curl, CURLMOPT_SOCKETFUNCTION,
                          &socket_callback);
        curl_multi_setopt(this->_curl,
                          CURLMOPT_TIMERFUNCTION, &Service::timeout_callback);
        // Pipelining causes issues with S3, requests end up being stuck.
        // curl_multi_setopt(this->_curl, CURLMOPT_PIPELINING, 1L);
      }

      Service::~Service()
      {
        if (--curl_global_ref_count() == 0)
          curl_global_cleanup();
      }

      void
      Service::shutdown_service()
      {
        auto res = curl_multi_cleanup(this->_curl);
        assert(res == CURLM_OK);
      }

      /*--------.
      | Request |
      `--------*/

      void
      Service::add(Request& request)
      {
        ELLE_TRACE_SCOPE("%s: start %s", *this, request);
        CURL* handle = request._impl->_handle;
        ELLE_ASSERT_NCONTAINS(this->_requests, handle);
        this->_requests.insert(std::make_pair(handle, request._impl));
        auto res = curl_multi_add_handle(this->_curl, handle);
        if (res != CURLM_OK)
          throw RequestError(request.url(),
                             elle::sprintf(
                               "%s: unable to register HTTP request: %s",
                               *this, curl_multi_strerror(res)));
        int running;
        res = curl_multi_socket_action(this->_curl,
                                       CURL_SOCKET_TIMEOUT, 0, &running);
        if (res != CURLM_OK)
          throw RequestError(request.url(),
                             elle::sprintf(
                               "%s: unable to start HTTP request: %s",
                               *this, curl_multi_strerror(res)));
        this->_pull_messages();
      }

      void
      Service::remove(Request& request)
      {
        auto handle = request._impl->_handle;
        ELLE_ASSERT_CONTAINS(this->_requests, handle);
        // Remove the request from the map only after curl_multi_remove_handle,
        // because it might run callbacks that need the request.
        if (this->_requests.find(handle) != this->_requests.end())
        {
          ELLE_TRACE_SCOPE("%s: remove %s", *this, request);
          auto res = curl_multi_remove_handle(this->_curl, handle);
          this->_requests.erase(handle);
          ELLE_ASSERT_EQ(res, CURLM_OK);
        }
        else
          ELLE_ASSERT(false);
      }

      /*-------.
      | Socket |
      `-------*/

      Service::SocketPtr
      Service::socket(int fd)
      {
        auto it = Socket::_sockets.find(fd);
        ELLE_ASSERT(it != Socket::_sockets.end());
        return SocketPtr(it->second);
      }

      /// Simple bouncer from the C-style CURL callback to handle_socket_action.
      int
      Service::socket_callback(CURL *easy,
                               curl_socket_t socket,
                               int action,
                               void* userp,
                               void*)
      {
        auto* self = reinterpret_cast<Service*>(userp);
        self->handle_socket_action(easy, socket, action);
        return 0;
      }

      /// Callback called when a socket is ready to read or write.
      void
      Service::handle_socket_ready(Request::Impl& request,
                                   SocketPtr socket,
                                   int action,
                                   boost::system::error_code const& error,
                                   size_t)
      {
        int running;
        if (error == boost::system::errc::operation_canceled)
        {
          // Callbacks are canceled when the events a request wait upon are
          // changed, ignore.
          return;
        }
        else if (error)
        {
          // Notify CURL of the error.
          ELLE_WARN("%s: %s has error: %s",
                    *this, *request._request, error.message());
          auto res = curl_multi_socket_action(this->_curl,
                                              socket->native_handle(),
                                              CURL_CSELECT_ERR,
                                              &running);
          this->_pull_messages();
          ELLE_ASSERT_EQ(res, CURLM_OK);
        }
        else
        {
          // Notify CURL that the socket is ready for reading or writing.
          ELLE_DEBUG_SCOPE("%s: %s (fd: %s) can %s",
                           *this, *request._request,
                           socket, action_string(action));
          auto res = curl_multi_socket_action(this->_curl,
                                              socket->native_handle(),
                                              action, &running);
          ELLE_ASSERT_EQ(res, CURLM_OK);
          this->_pull_messages();
        }
        // Re-register the same callback: CURL will call handle_socket_action to
        // notify us when a socket does not need reading/writing anymore. Until
        // then, keep monitoring and notifying CURL.
        switch (action)
        {
          case CURL_CSELECT_OUT:
            this->register_socket_write(request, socket);
            break;
          case CURL_CSELECT_IN:
            this->register_socket_read(request, socket);
            break;
        }
      };

      /// Called by CURL to inform us of what a request is waiting for (reading,
      /// writing, none).
      void
      Service::handle_socket_action(CURL* handle,
                                    curl_socket_t socket,
                                    int action)
      {
        // Find the Request.
        if (this->_requests.find(handle) == this->_requests.end())
        {
          ELLE_ERR("missing request: %s, %s", handle, socket);
          return;
        }
        Request::Impl& request = *this->_requests.find(handle)->second;
        ELLE_DEBUG_SCOPE("%s: %s (fd: %s) wants to %s",
                         *this, request, socket, action_string(action));
        auto sock = request._curl.socket(socket);
        // Set status and register needed callbacks.
        sock->writing = action & CURL_POLL_OUT;
        sock->reading = action & CURL_POLL_IN;
        // Cancel previous callbacks.
        sock->cancel();
        // Re-register needed callbacks.
        this->register_socket_write(request, sock);
        this->register_socket_read(request, sock);
      }

      /// Register write event if the request is writing.
      void
      Service::register_socket_write(Request::Impl& request,
                                     SocketPtr socket)
      {
        if (!socket->writing)
          return;
        ELLE_DEBUG_SCOPE("%s: register %s (fd: %s) for writing",
                         *this, *request._request, socket->native_handle());
        socket->async_write_some(boost::asio::null_buffers(),
                                 std::bind(&Service::handle_socket_ready,
                                           std::ref(*this),
                                           std::ref(request),
                                           socket,
                                           CURL_CSELECT_OUT,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
      }

      /// Register read event if the request is writing.
      void
      Service::register_socket_read(Request::Impl& request,
                                    SocketPtr socket)
      {
        if (!socket->reading)
          return;
        ELLE_DEBUG_SCOPE("%s: register %s (fd: %s) for reading",
                         *this, *request._request, socket->native_handle());
        socket->async_read_some(boost::asio::null_buffers(),
                                std::bind(&Service::handle_socket_ready,
                                          std::ref(*this),
                                          std::ref(request),
                                          socket,
                                          CURL_CSELECT_IN,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
      }

      /*--------.
      | Timeout |
      `--------*/

      int
      Service::timeout_callback(CURLM*,
                                long ms,
                                void* userp)
      {
        auto* self = reinterpret_cast<Service*>(userp);
        if (ms == -1)
          self->unset_timeout();
        else
          self->set_timeout(boost::posix_time::milliseconds(ms));
        return 0;
      }

      void
      Service::set_timeout(boost::posix_time::time_duration const& timeout)
      {
        ELLE_DEBUG("%s: set timeout to %s", *this, timeout);
        this->_timer.cancel();
        this->_timer.expires_from_now(timeout);
        this->_timer.async_wait(std::bind(&Service::handle_timeout,
                                          std::ref(*this),
                                          std::placeholders::_1));
      }

      void
      Service::unset_timeout()
      {
        ELLE_DEBUG("%s: cancel timeout", *this);
        this->_timer.cancel();
      }

      void
      Service::handle_timeout(const boost::system::error_code& error)
      {
        if (error == boost::system::errc::operation_canceled)
          return;
        ELLE_ASSERT(!error);
        int running = 0;
        ELLE_DEBUG_SCOPE("%s: timeout", *this);
        auto res = curl_multi_socket_action(this->_curl,
                                            CURL_SOCKET_TIMEOUT, 0, &running);
        this->_pull_messages();
        ELLE_ASSERT_EQ(res, CURLM_OK);
      }

      /*-----.
      | Info |
      `-----*/

      void
      Service::_pull_messages()
      {
        ELLE_DEBUG_SCOPE("%s: check CURL messages", *this);
        int left;
        while (CURLMsg* msg = curl_multi_info_read(this->_curl, &left))
        {
          auto handle = msg->easy_handle;
          ELLE_ASSERT_CONTAINS(this->_requests, handle);
          Request::Impl& request = *this->_requests.find(handle)->second;
          CURLcode res = msg->data.result;
          if (msg->msg == CURLMSG_DONE)
          {
            ELLE_TRACE("%s: %s is complete with code %s", *this, request, res);
            request._request->_complete(res);
            this->remove(*request._request);
          }
          else
          {
            ELLE_WARN("%s: got an unknown message: %d", *this, msg->msg);
          }
        }
      }

      /*----------.
      | Printable |
      `----------*/

      void
      Service::print(std::ostream& stream) const
      {
        stream << "Service";
      }

      std::unordered_map<int, std::weak_ptr<Service::Socket>>
      Service::Socket::_sockets;
    }
  }
}
