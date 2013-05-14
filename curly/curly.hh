#ifndef ELLE_CURLY_HH
# define ELLE_CURLY_HH

# include <chrono>
# include <iosfwd>
# include <map>
# include <memory>

# include <curl/curl.h>

#define throw_if_ecode(easy, code) _throw_if_ecode((easy), (code))

namespace curly
{
  void
  _throw_if_ecode(CURL* easy,
                  CURLcode code,
                  std::string const& error_message = "");

  class curl_easy_deleter
  {
  public:
    curl_easy_deleter() = default;
    void 
    operator ()(CURL* ptr);
  };

  class curl_slist_deleter
  {
  public:
    curl_slist_deleter() = default;
    void 
    operator ()(struct curl_slist* ptr);
  };

  class request_configuration
  {
  private:
    // These requests class will fire the request:
    // so we need to give them access to the CURL handle.
    friend class request;
    friend class asio_request;
    friend class curl_service;

    std::shared_ptr<CURL> _easy_handle;
    std::shared_ptr<struct curl_slist> _header_list = nullptr;

    // I/O of the body.
    std::ostream* _output = nullptr;
    std::istream* _input = nullptr;

    CURL*
    native_handle();

    std::string
    header(std::pair<std::string, std::string> const&p);
  public:
    request_configuration();
    request_configuration(request_configuration&&) = default;
    request_configuration(request_configuration const&) = default;
    ~request_configuration() = default;

    // Configuration helpers.
    void
    url(std::string const&);
    void
    output(std::ostream& out);
    void
    input(std::istream& out);
    void
    headers(std::map<std::string, std::string> const& m);
    void
    user_agent(std::string const& ua);
    void
    cookie(std::string const& ua);
    void
    referer(std::string const& ua);

    // Set the given CURLOption. Forward to curl_easy_setopt.
    template <typename T>
    int
    option(CURLoption opt,
           T&& param);
  };

  class request
  {
  private:
    request_configuration _config;
    int _response_code = -1;

    static
    size_t
    write_helper(char* ptr,
                 size_t size,
                 size_t nmemb,
                 void* userptr);
    static
    size_t
    read_helper(char* ptr,
                size_t size,
                size_t nmemb,
                void* userptr);
  public:
    request(request_configuration c);
    ~request() = default;

    // Retrieve info about the request. Forward to curl_easy_getinfo.
    template <typename T>
    int
    info(CURLINFO info, T&& param);

    // Return the protocol response code, if any.
    int
    code() const;

    // Return the last effective url.
    std::string
    url();

    // Return the duration of the request in seconds.
    std::chrono::duration<double>
    time();
  };

  // Shortcut to do quick requests.
  // Return the body.
  std::string
  get(std::string const& url);

  // Write the body into out.
  void
  get(std::string const& url,
      std::ostream& out);

  // Read the body from in.
  std::string
  post(std::string const& url,
       std::istream& in);

  // Read the body from in, and write the body to out.
  void
  post(std::string const& url,
       std::istream& in,
       std::ostream& out);

  // Read the body from in.
  std::string
  put(std::string const& url,
      std::istream& in);

  // Read the body from in, and write the body to out.
  void
  put(std::string const& url,
      std::istream& in,
      std::ostream& out);

  // Shortcuts to build request_configurations:

  // Theses shortcut construct basic configuration with default options.
  request_configuration
  make_get();
  request_configuration
  make_post();
  request_configuration
  make_put();

} /* curly */

# include <curly.hxx>

#endif /* end of include guard: ELLE_CURLY_HH_ */
