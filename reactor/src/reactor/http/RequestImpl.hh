#ifndef REACTOR_HTTP_REQUEST_IMPL_HH
# define REACTOR_HTTP_REQUEST_REQUEST_IMPL_HH

# include <queue>
# include <string>

# include <elle/Buffer.hh>
# include <elle/memory.hh>

# include <reactor/Barrier.hh>
# include <reactor/http/fwd.hh>
# include <reactor/http/Request.hh>
# include <reactor/signal.hh>

namespace reactor
{
  namespace http
  {
    class Request::Impl:
      public elle::StreamBuffer
    {
    public:
      Impl(Request& request,
           std::string const& url,
           Method method,
           Configuration conf);
      ~Impl();
      void
      start();

      ELLE_ATTRIBUTE(Request*, request);
      ELLE_ATTRIBUTE(Configuration, conf);
      ELLE_ATTRIBUTE(elle::generic_unique_ptr<curl_slist>, headers);

    /*--------.
    | Headers |
    `--------*/
    public:
      void
      header_add(std::string const& header, std::string const& content);
      void
      header_remove(std::string const& header);

    /*-------.
    | Socket |
    `-------*/
    public:
      static
      int
      open_socket(void* data,
                  curlsocktype purpose,
                  struct curl_sockaddr *address);
      static
      int
      close_socket(void*, curl_socket_t);

    /*-------------.
    | StreamBuffer |
    `-------------*/
    protected:
      elle::WeakBuffer
      write_buffer() override;
      void
      flush(unsigned int size) override;
      elle::WeakBuffer
      read_buffer() override;
      static
      size_t
      write_callback(char* ptr, size_t chunk, size_t count, void* userdata);
      void
      enqueue_data(elle::Buffer buffer);

    private:
      void
      _complete();
      bool _input_done;
      std::queue<elle::Buffer> _input;
      elle::Buffer _input_current;
      reactor::Barrier _input_available;
      bool _output_done;
      elle::Buffer _output;
      bool _output_available;
      reactor::Signal _output_consumed;
      int _output_offset;

    /*------.
    | Input |
    `------*/
    private:
      static
      size_t
      read_callback(char* ptr, size_t chunk, size_t count, void* userdata);
      size_t
      read_data(elle::WeakBuffer buffer);
      /// XXX: For HTTP 1.0 servers. See comment in Request constructor.
      std::unique_ptr<std::stringstream> _input_buffer;

    private:
      friend class Client;
      friend class Request;
      friend class Service;
      friend
      CURL*
      handle_from_request(Request const& r);
      Service& _curl;
      std::string _url;
      Method _method;
      CURL* _handle;
      std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
      char _error[CURL_ERROR_SIZE];
      ELLE_ATTRIBUTE_R(int, pause_count);
    };
  }
}

#endif
