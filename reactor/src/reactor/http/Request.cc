#include <curl/curl.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find_iterator.hpp>

#include <elle/Exception.hh>
#include <elle/log.hh>

#include <reactor/http/exceptions.hh>
#include <reactor/http/RequestImpl.hh>
#include <reactor/http/Service.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.http.Request");

namespace reactor
{
  namespace http
  {
    /*--------.
    | Helpers |
    `--------*/

    template <typename Value>
    static
    void
    setopt(CURL* handle, CURLoption option, Value parameter)
    {
      auto res = curl_easy_setopt(handle, option, parameter);
      if (res != CURLE_OK)
        throw RequestError("unable to set request option: %s",
                           curl_easy_strerror(res));
    }

    /*-------.
    | Method |
    `-------*/

    std::ostream&
    operator <<(std::ostream& output,
                Method method)
    {
      switch (method)
      {
        case Method::DELETE:
          output << "DELETE";
          break;
        case Method::GET:
          output << "GET";
          break;
        case Method::POST:
          output << "POST";
          break;
        case Method::PUT:
          output << "PUT";
          break;
        default:
          elle::unreachable();
      }
      return output;
    }

    /*--------.
    | Version |
    `--------*/

    std::ostream&
    operator <<(std::ostream& output,
                Version version)
    {
      switch (version)
      {
        case Version::v10:
          output << "HTTP/1.0";
          break;
        case Version::v11:
          output << "HTTP/1.1";
          break;
        default:
          elle::unreachable();
      }
      return output;
    }

    /*-----------.
    | StatusCode |
    `-----------*/

    /// Pretty print a HTTP status code.
    std::ostream&
    operator <<(std::ostream& output,
                StatusCode status)
    {
      switch (status)
      {
        case StatusCode::Continue:
          output << "Continue";
          break;
        case StatusCode::Switching_Protocols:
          output << "Switching Protocols";
          break;
        case StatusCode::OK:
          output << "OK";
          break;
        case StatusCode::Created:
          output << "Created";
          break;
        case StatusCode::Accepted:
          output << "Accepted";
          break;
        case StatusCode::Non_Authoritative_Information:
          output << "Non Authoritative Information";
          break;
        case StatusCode::No_Content:
          output << "No Content";
          break;
        case StatusCode::Reset_Content:
          output << "Reset Content";
          break;
        case StatusCode::Partial_Content:
          output << "Partial Content";
          break;
        case StatusCode::Multiple_Choices:
          output << "Multiple Choices";
          break;
        case StatusCode::Moved_Permanently:
          output << "Moved Permanently";
          break;
        case StatusCode::Found:
          output << "Found";
          break;
        case StatusCode::See_Other:
          output << "See Other";
          break;
        case StatusCode::Not_Modified:
          output << "Not Modified";
          break;
        case StatusCode::Use_Proxy:
          output << "Use Proxy";
          break;
        case StatusCode::Unused:
          output << "Unused";
          break;
        case StatusCode::Temporary_Redirect:
          output << "Temporary Redirect";
          break;
        case StatusCode::Bad_Request:
          output << "Bad Request";
          break;
        case StatusCode::Unauthorized:
          output << "Unauthorized";
          break;
        case StatusCode::Payment_Required:
          output << "Payment Required";
          break;
        case StatusCode::Forbidden:
          output << "Forbidden";
          break;
        case StatusCode::Not_Found:
          output << "Not Found";
          break;
        case StatusCode::Method_Not_Allowed:
          output << "Method Not Allowed";
          break;
        case StatusCode::Not_Acceptable:
          output << "Not Acceptable";
          break;
        case StatusCode::Proxy_Authentication_Required:
          output << "Proxy Authentication Required";
          break;
        case StatusCode::Request_Timeout:
          output << "Request Timeout";
          break;
        case StatusCode::Conflict:
          output << "Conflict";
          break;
        case StatusCode::Gone:
          output << "Gone";
          break;
        case StatusCode::Length_Required:
          output << "Length Required";
          break;
        case StatusCode::Precondition_Failed:
          output << "Precondition Failed";
          break;
        case StatusCode::Request_Entity_Too_Large:
          output << "Request Entity Too Large";
          break;
        case StatusCode::Request_URI_Too_Long:
          output << "Request-URI Too Long";
          break;
        case StatusCode::Unsupported_Media_Type:
          output << "Unsupported Media Type";
          break;
        case StatusCode::Requested_Range_Not_Satisfiable:
          output << "Requested Range Not Satisfiable";
          break;
        case StatusCode::Expectation_Failed:
          output << "Expectation Failed";
          break;
        case StatusCode::Internal_Server_Error:
          output << "Internal Server Error";
          break;
        case StatusCode::Not_Implemented:
          output << "Not Implemented";
          break;
        case StatusCode::Bad_Gateway:
          output << "Bad Gateway";
          break;
        case StatusCode::Service_Unavailable:
          output << "Service Unavailable";
          break;
        case StatusCode::Gateway_Timeout:
          output << "Gateway Timeout";
          break;
        case StatusCode::HTTP_Version_Not_Supported:
          output << "HTTP Version Not Supported";
          break;
      }
      return output;
    }

    /*--------------.
    | Configuration |
    `--------------*/

    Request::Configuration::Configuration(DurationOpt timeout,
                                          Version version):
      _version(version),
      _timeout(timeout),
      _headers(),
      // XXX: not supported by wsgiref and <=nginx-1.2 ...
      _chunked_transfers(false)
    {}

    Request::Configuration::~Configuration()
    {}

    void
    Request::Configuration::header_add(std::string const& header,
                              std::string const& content)
    {
      this->_headers.insert(std::make_pair(header, content));
    }

    void
    Request::Configuration::header_remove(std::string const& header)
    {
      this->_headers.erase(header);
    }

    /*---------------.
    | Implementation |
    `---------------*/

    Request::Impl::Impl(Request& request,
                        std::string const& url,
                        Method method,
                        Configuration conf):
      _request(&request),
      _conf(std::move(conf)),
      _headers(nullptr, &curl_slist_free_all),
      _input_done(false),
      _input(),
      _input_current(),
      _input_available(),
      _output_done(false),
      _output(0),
      _output_available(false),
      _output_offset(0),
      _curl(boost::asio::use_service<Service>(
              Scheduler::scheduler()->io_service())),
      _url(url),
      _method(method),
      _handle(curl_easy_init()),
      _pause_count(0)
    {
      if (!this->_handle)
        throw RequestError(url, "unable to initialize request");
      // Set error buffer.
      memset(this->_error, 0, CURL_ERROR_SIZE);
      setopt(this->_handle, CURLOPT_ERRORBUFFER, this->_error);
      // Set version.
      auto version = this->_conf.version() == Version::v11 ?
        CURL_HTTP_VERSION_1_1 : CURL_HTTP_VERSION_1_0;
      setopt(this->_handle, CURLOPT_HTTP_VERSION, version);
      // Set timeout.
      auto const& timeout = this->_conf.timeout();
      auto timeout_seconds = timeout ?
        std::max(timeout->total_seconds(), 1) : 0;
      setopt(this->_handle, CURLOPT_TIMEOUT, timeout_seconds);
      // Set URL.
      setopt(this->_handle, CURLOPT_URL, url.c_str());
      // Set method.
      switch (this->_method)
      {
      case Method::DELETE:
        setopt(this->_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
      case Method::GET:
        setopt(this->_handle, CURLOPT_HTTPGET, 1);
        break;
      case Method::POST:
        setopt(this->_handle, CURLOPT_POST, 1);
        break;
      case Method::PUT:
        setopt(this->_handle, CURLOPT_UPLOAD, 1);
        break;
      }
      // Set socket callbacks.
      setopt(this->_handle, CURLOPT_OPENSOCKETFUNCTION, &Impl::open_socket);
      setopt(this->_handle, CURLOPT_OPENSOCKETDATA, this);
      setopt(this->_handle, CURLOPT_CLOSESOCKETFUNCTION, &Impl::close_socket);
      setopt(this->_handle, CURLOPT_CLOSESOCKETDATA, &this->_curl);
      // Set input / output callbacks.
      setopt(this->_handle, CURLOPT_WRITEFUNCTION, &Impl::write_callback);
      setopt(this->_handle, CURLOPT_WRITEDATA, this);
      setopt(this->_handle, CURLOPT_READFUNCTION, &Impl::read_callback);
      setopt(this->_handle, CURLOPT_READDATA, this);
      // Set header callbacks
      setopt(this->_handle, CURLOPT_HEADERFUNCTION, &Impl::header_callback);
      setopt(this->_handle, CURLOPT_WRITEHEADER, this);
      // XXX: Don't check SSL peer until we find a solution for managing trusted
      // CAs.
      // http://bugs.python.org/issue17128
      setopt(this->_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    }

    Request::Impl::~Impl()
    {
      ELLE_TRACE_SCOPE("%s: terminate", *this->_request);
      if (this->_curl._requests.find(this->_handle) != this->_curl._requests.end())
        this->_curl.remove(*this->_request);
      curl_easy_cleanup(this->_handle);
    }

    std::unordered_map<std::string, std::string>
    Request::Impl::cookies(CURL* handle)
    {
      std::unordered_map<std::string, std::string> cookies;
      CURLcode res;
      struct curl_slist* cookies_list = nullptr;
      res = curl_easy_getinfo(handle,
                              CURLINFO_COOKIELIST,
                              &cookies_list);
      if (res != CURLE_OK)
        throw elle::Exception(elle::sprintf("retrieval of cookies failed: %s",
                                            curl_easy_strerror(res)));
      for (curl_slist* it = cookies_list; it; it = it->next)
      {
        std::vector<std::string> chunks;
        boost::algorithm::split(chunks, it->data,
                                boost::algorithm::is_any_of("\t"));
        auto name = chunks[chunks.size() - 2];
        auto value = chunks[chunks.size() - 1];
        cookies[name] = value;
      }
      curl_slist_free_all(cookies_list);
      return cookies;
    }

    std::unordered_map<std::string, std::string>
    Request::Impl::cookies() const
    {
      return this->cookies(this->_handle);
    }

    void
    Request::Impl::cookie_add(std::string const& name,
                              std::string const& value)
    {
      static boost::format const fmt("Set-Cookie: %s=%s; path=/;");
      auto line = str(boost::format(fmt) % name % value);
      auto res = curl_easy_setopt(this->_handle,
                                  CURLOPT_COOKIELIST, line.c_str());
      if (res != CURLE_OK)
        throw elle::Exception(elle::sprintf("failed to set cookie: %s",
                                            curl_easy_strerror(res)));
    }

    void
    Request::Impl::start()
    {
      for (auto const& cookie: this->_conf.cookies())
        this->cookie_add(cookie.first, cookie.second);
      for (auto const& header: this->_conf.headers())
        this->header_add(header.first, header.second);
      setopt(this->_handle, CURLOPT_HTTPHEADER, this->_headers.get());
      this->_curl.add(*this->_request);
    }

    /*--------.
    | Headers |
    `--------*/

    void
    Request::Impl::header_add(std::string const& header,
                              std::string const& content)
    {
      ELLE_DEBUG("%s: add header %s: %s", *this, header, content);
      auto line = elle::sprintf("%s: %s", header, content);
      this->_headers.reset(curl_slist_append(this->_headers.release(),
                                             line.c_str()));
    }

    void
    Request::Impl::header_remove(std::string const& header)
    {
      ELLE_DEBUG("%s: remove header %s", *this, header);
      auto line = elle::sprintf("%s:", header);
      this->_headers.reset(curl_slist_append(this->_headers.release(),
                                             line.c_str()));
    }

    size_t
    Request::Impl::header_callback(void *ptr,
                                   size_t chunks,
                                   size_t count,
                                   void* userdata)
    {
      Request::Impl& self = *reinterpret_cast<Request::Impl*>(userdata);
      auto size = chunks * count;
      self.read_header(elle::WeakBuffer(ptr, size));
      return size;
    }

    void
    Request::Impl::read_header(elle::ConstWeakBuffer const& data)
    {
      auto separator =
        boost::algorithm::find_first(data, elle::ConstWeakBuffer(":"));
      if (separator.begin() != data.end())
        this->_request->_headers.insert(
          std::make_pair(
            std::string(data.begin(), separator.begin()),
            boost::algorithm::trim_copy(
              std::string(separator.end(), data.end()))));
    }

    /*-------.
    | Socket |
    `-------*/

    int
    Request::Impl::open_socket(void* data,
                               curlsocktype purpose,
                               struct curl_sockaddr *address)
    {
      Request::Impl& self = *reinterpret_cast<Request::Impl*>(data);
      if (purpose == CURLSOCKTYPE_IPCXN)
      {
        if (address->family != AF_INET && address->family != AF_INET6)
          return CURL_SOCKET_BAD;
        auto proto = address->family == AF_INET ?
          boost::asio::ip::tcp::v4(): boost::asio::ip::tcp::v6();
        auto& service = self._curl.get_io_service();
        auto socket =
          std::make_shared<Service::Socket>(service, proto);
        auto fd = socket->native_handle();
        ELLE_TRACE_SCOPE("%s: open socket %s for %s", self._curl, fd, self);
        Service::Socket::_sockets.insert(
          std::make_pair(fd, socket));
        ELLE_ASSERT_NCONTAINS(self._curl._sockets, fd);
        self._curl._sockets.insert(std::make_pair(fd, socket));
        return fd;
      }
      else
        return CURL_SOCKET_BAD;
    }

    int
    Request::Impl::close_socket(void* data, curl_socket_t fd)
    {
      // Beware, curl_multi keeps reference to the socket and destroy it *after*
      // the Request and its easy handle have been disposed of. The request is
      // thus invalid in this concept, hence the use of a map in the Service to
      // hold sockets. It seems that Curl does this to reuse sockets between
      // requests in some cases.
      Service& service = *reinterpret_cast<Service*>(data);
      ELLE_TRACE_SCOPE("%s: close socket %s", service, fd);
      ELLE_ASSERT_CONTAINS(service._sockets, fd);
      ELLE_ENFORCE(service._sockets.erase(fd));
      return 0;
    }

    /*-------------.
    | StreamBuffer |
    `-------------*/

    elle::WeakBuffer
    Request::Impl::write_buffer()
    {
      ELLE_ASSERT(!this->_output_done);
      if (this->_conf.chunked_transfers())
      {
        this->_output.size(CURL_MAX_WRITE_SIZE);
        return this->_output;
      }
      else
      {
        if (this->_output_available)
          this->_output_consumed.wait();
        this->_output.capacity(this->_output.size() + CURL_MAX_WRITE_SIZE);
        return elle::WeakBuffer(
          this->_output.mutable_contents() + this->_output.size(),
          CURL_MAX_WRITE_SIZE);
      }
    }

    void
    Request::Impl::flush(unsigned int size)
    {
      ELLE_ASSERT(!this->_output_done);
      if (this->_conf.chunked_transfers())
      {
        this->_output.size(size);
        ELLE_DEBUG_SCOPE("%s: output: post data: %s",
                         *this->_request, this->_output);
        this->_output_available = true;
        curl_easy_pause(this->_handle, CURLPAUSE_CONT);
      }
      else
      {
        elle::ConstWeakBuffer newdata(
          this->_output.mutable_contents() + this->_output.size(), size);
        ELLE_DEBUG_SCOPE("%s: output: post data: %s",
                         *this->_request, newdata);
        this->_output.size(this->_output.size() + size);
      }
    }

    elle::WeakBuffer
    Request::Impl::read_buffer()
    {
      if (!this->_input_available.opened())
      {
        ELLE_DEBUG_SCOPE("%s: input: wait for more data", *this->_request);
        this->_input_available.wait();
      }
      if (!this->_input.empty())
      {
        this->_input_current = std::move(this->_input.front());
        ELLE_DEBUG_SCOPE("%s: input: fetch data: %f",
                         *this->_request, this->_input_current);
        ELLE_DUMP("%s", this->_input_current);
        this->_input.pop();
        if (this->_input.empty() && !this->_input_done)
          this->_input_available.close();
        return this->_input_current;
      }
      else
      {
        ELLE_DEBUG("%s: input: end of data", *this->_request);
        return elle::WeakBuffer();
      }
    }

    size_t
    Request::Impl::write_callback(char* ptr,
                                  size_t chunk,
                                  size_t count,
                                  void* userdata)
    {
      Request::Impl& self = *reinterpret_cast<Request::Impl*>(userdata);
      auto size = chunk * count;
      self.enqueue_data(elle::Buffer(ptr, size));
      return size;
    }

    void
    Request::Impl::enqueue_data(elle::Buffer buffer)
    {
      ELLE_DEBUG_SCOPE("%s: input: got data: %f", *this->_request, buffer);
      this->_input.push(std::move(buffer));
      this->_input_available.open();
    }

    void
    Request::Impl::_complete()
    {
      this->_input_available.open();
      this->_input_done = true;
    }

    /*------.
    | Input |
    `------*/
    size_t
    Request::Impl::read_callback(char* ptr,
                                 size_t chunk,
                                 size_t count,
                                 void* userdata)
    {
      Request::Impl& self = *reinterpret_cast<Request::Impl*>(userdata);
      auto size = chunk * count;
      return self.read_data(elle::WeakBuffer(ptr, size));
    }

    size_t
    Request::Impl::read_data(elle::WeakBuffer buffer)
    {
      if (this->_conf.chunked_transfers())
      {
        ELLE_ASSERT_GTE(buffer.size(), this->_output.size());
        if (!this->_output_available)
        {
          if (this->_output_done)
          {
            ELLE_DEBUG("%s: output: end of data", *this->_request);
            return 0;
          }
          ELLE_DEBUG("%s: output: no data available, pause", *this->_request);
          ++this->_pause_count;
          return CURL_READFUNC_PAUSE;
        }
        this->_output_available = false;
        memcpy(buffer.mutable_contents(),
               this->_output.mutable_contents(), this->_output.size());
        this->_output_consumed.signal();
        ELLE_DEBUG("%s: output: get %s bytes",
                   *this->_request, this->_output.size());
        return this->_output.size();
      }
      else
      {
        ELLE_ASSERT(this->_output_done);
        elle::ConstWeakBuffer source(
          this->_output.contents() + this->_output_offset,
          this->_output.size() - this->_output_offset);
        if (source.size() == 0)
        {
          ELLE_DEBUG("%s: output: end of data", *this->_request);
          return 0;
        }
        auto effective = std::min(source.size(), buffer.size());
        memcpy(buffer.mutable_contents(),
               source.contents(), effective);
        this->_output_offset += effective;
        ELLE_DEBUG("%s: output: get %s bytes", *this->_request, effective);
        return effective;
      }
    }

    /*---------.
    | Printing |
    `---------*/

    void
    Request::Impl::print(std::ostream& stream) const
    {
      stream << this->_method << " on " << this->_url
             << " (" << this->_handle << ")";
    }

    /*-------------.
    | Construction |
    `-------------*/

    Request::Request(std::string const& url,
                     Method method,
                     Configuration conf,
                     bool):
      elle::IOStream(this->_impl = new Impl(*this, url, method,
                                            std::move(conf))),
      _method(method),
      _url(url),
      _status(static_cast<StatusCode>(0))
    {}

    Request::Request(std::string const& url,
                     Method method,
                     Configuration conf):
      Request(url, method, std::move(conf), true)
    {
      this->_impl->header_remove("Transfer-Encoding");
      if (conf.chunked_transfers())
        this->_impl->start();
    }

    Request::Request(std::string const& url,
                     Method method,
                     std::string const& content_type,
                     Configuration conf):
      Request(url, method, std::move(conf), true)
    {
      this->_impl->header_add("Content-Type", content_type);
      // XXX: HTTP/1.0 expects a Content-Size, preventing streaming. Be safe for
      // now.
      if (conf.chunked_transfers())
      {
        this->_impl->header_add("Transfer-Encoding", "chunked");
        this->_impl->start();
      }
      else
        this->_impl->header_remove("Transfer-Encoding");
    }

    /// Move a Request.
    Request::Request(Request&& source):
      elle::IOStream(std::move(source)),
      _method(source._method),
      _url(source._url),
      _impl(source._impl)
    {
      source._impl = nullptr;
      this->_impl->_request = this;
    }

    Request::~Request()
    {
      // The impl is deleted as the StreamBuffer.
    }

    /*-----------.
    | Completion |
    `-----------*/

    void
    Request::finalize()
    {
      if (this->_impl->_output_done)
        return;
      ELLE_TRACE_SCOPE("%s: output: finalize", *this);
      this->flush();
      this->_impl->_output_done = true;
      curl_easy_pause(this->_impl->_handle, CURLPAUSE_CONT);
      if (!this->_impl->_conf.chunked_transfers())
      {
        this->_impl->header_add(
          "Content-Length",
          boost::lexical_cast<std::string>(this->_impl->_output.size()));
        this->_impl->start();
      }
    }

    void
    Request::_complete(int code)
    {
      curl_easy_getinfo(this->_impl->_handle,
                        CURLINFO_RESPONSE_CODE, &this->_status);
      std::string message;
      auto set_exception = [&]
        {
          if (code == CURLE_GOT_NOTHING)
            this->_raise<EmptyResponse>(this->_url);
          else if (code == CURLE_OPERATION_TIMEDOUT)
            this->_raise<Timeout>(this->_url,
                                  this->_impl->_conf.timeout().get());
          else
            this->_raise<RequestError>(this->_url, message);
        };
      if (code != CURLE_OK)
      {
        message = elle::sprintf("%s: %s",
                                curl_easy_strerror(CURLcode(code)),
                                this->_impl->_error);
        ELLE_WARN("%s: done with error: %s", *this, message);
        set_exception();
      }
      else
        ELLE_TRACE_SCOPE("%s: done with status %s", *this, this->_status);
      this->_signal();
      // Waitables consume their exception once signaled, restore it.
      if (code != CURLE_OK)
        set_exception();
      this->_impl->_complete();
    }

    bool
    Request::_wait(Thread* thread)
    {
      this->finalize();
      if (this->_impl->_input_done)
      {
        if (std::exception_ptr exn = this->exception())
          std::rethrow_exception(exn);
        return false;
      }
      else
        return Waitable::_wait(thread);
    }

    /*-------.
    | Status |
    `-------*/

    StatusCode
    Request::status() const
    {
      // XXX: We need not wait for the whole request.
      const_cast<Request*>(this)->wait();
      ELLE_ASSERT_NEQ(this->_status, static_cast<StatusCode>(0));
      return this->_status;
    }

    elle::Buffer
    Request::response()
    {
      this->finalize();
      elle::Buffer res;
      {
        elle::IOStream output(new elle::OutputStreamBuffer(res));
        std::copy(std::istreambuf_iterator<char>(*this),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(output));
      }
      // We ran out of data so the request should be finished; this is just for
      // exceptions.
      this->wait();
      return res;
    }

    int
    Request::pause_count() const
    {
      return this->_impl->_pause_count;
    }

    /*--------.
    | Cookies |
    `--------*/

    Request::Configuration::Cookies
    Request::cookies() const
    {
      return this->_impl->cookies();
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Request::print(std::ostream& stream) const
    {
      stream << *this->_impl;
    }

    /*----------.
    | Shortcuts |
    `----------*/

    elle::Buffer
    get(std::string const& url,
        Request::Configuration conf)
    {
      Request r(url, Method::GET, std::move(conf));
      return r.response();
    }
  }
}
