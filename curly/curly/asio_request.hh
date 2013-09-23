#ifndef CURLY_ASIO_REQUEST_HH
# define CURLY_ASIO_REQUEST_HH

# include <functional>

# include <boost/asio/ip/tcp.hpp>

# include <curly/curly.hh>
# include <curly/curly_service.hh>

namespace curly
{
  class asio_request
  {
  public:
    friend class curl_service;
    using completion_callback = std::function<void (int code)>;
    completion_callback _callback;

  private:
    curl_service& _io;
    curly::request_configuration _config;
    boost::asio::ip::tcp::socket _socket;
    char _error[CURL_ERROR_SIZE];
    // CURL next action
    int _next_action;

    static size_t
    write_helper(char* data,
                 size_t size, size_t nmemb,
                 void* userptr);

    static size_t
    read_helper(char* data,
                size_t size, size_t nmemb,
                void* userptr);

    static int
    open_socket(void *userptr,
                curlsocktype purpose,
                struct curl_sockaddr *address);

    static int
    close_socket(void *userptr,
                 curl_socket_t item);

  public:

    asio_request(boost::asio::io_service& io,
                 curly::request_configuration c);

    void
    async_perform(completion_callback const& f);

    curly::request_configuration const&
    config();

    std::string
    error_string() const;
  };

} /* curly */

#endif /* end of include guard: CURLY_ASIO_REQUEST_HH */
