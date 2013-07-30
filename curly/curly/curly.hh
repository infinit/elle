#ifndef ELLE_CURLY_HH
# define ELLE_CURLY_HH

# include <elle/attribute.hh>

# include <curl/curl.h>

# include <chrono>
# include <iosfwd>
# include <map>
# include <memory>
# include <string>

namespace curly
{
  void
  throw_if_ecode(CURL* easy,
                 CURLcode code,
                 std::string const& error_message);

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
    typedef std::unique_ptr<struct curl_slist, curl_slist_deleter> Headers;
    ELLE_ATTRIBUTE(Headers, headers);

    // I/O of the body.
    std::ostream* _output = nullptr;
    std::istream* _input = nullptr;

  public:
    request_configuration();
    request_configuration(request_configuration&&) = default;
    request_configuration(request_configuration const&) = delete;
    ~request_configuration() = default;

    CURL*
    native_handle() const;

    CURL*
    native_handle();

    // Configuration helpers.
    void
    url(std::string const&);
    void
    output(std::ostream& out);
    void
    input(std::istream& out);
    void
    header(std::string const& name,
           std::string const& value);
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

# include <curly/curly.hxx>

#endif /* end of include guard: ELLE_CURLY_HH_ */
