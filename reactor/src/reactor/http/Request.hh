#ifndef REACTOR_HTTP_REQUEST_HH
# define REACTOR_HTTP_REQUEST_HH

# include <string>
# include <unordered_map>

# include <boost/asio.hpp>
# include <boost/signals2.hpp>

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <reactor/http/fwd.hh>
# include <reactor/http/Method.hh>
# include <reactor/http/StatusCode.hh>
# include <reactor/http/Version.hh>
# include <reactor/network/proxy.hh>
# include <reactor/operation.hh>

namespace reactor
{
  namespace http
  {
    /*--------.
    | Request |
    `--------*/
    /// A HTTP request.
    ///
    /// A Request performs a HTTP request according to the configuration it is
    /// given and handle the server response.  It interfaces the outgoing and
    /// incoming body with a stream interface.  The HTTP request starts when the
    /// object is created and will progress as soon as possible, only waiting
    /// when data is required from the user or the server.
    ///
    /// The Request parameters are given through its constructor: needed
    /// fundamental information that are URL and HTTP method first, and a
    /// Configuration object that can tweak many other subtleties.
    ///
    /// If the request sends a body, it has to be fed through the output stream
    /// interface.  The data will be streamed on the fly to the server and this
    /// API thus may yield.  The end of the body must be signaled by calling
    /// finalize().
    ///
    /// The response body can be retreived through the input stream interface.
    /// Data are streamed on the fly from the server, so it is encouraged to
    /// start reading data immediately after the request has started, to avoid
    /// storing needless data in memory for too long.  In particular, it would
    /// be counterproductive to first Waitable::wait for the request to end and
    /// then read the body, as it would force its complete storage in memory
    /// while it could have been consumed as a stream.
    ///
    /// As a Waitable, the Request will block waiting threads until
    /// completion. Waiting a request automatically signals end of input as if
    /// finalize had been called.
    class Request:
      public reactor::Waitable,
      public elle::IOStream
    {
    /*--------------.
    | Configuration |
    `--------------*/
    public:
      /// Configuration of an HTTP request.
      class Configuration
      {
        using Proxy = reactor::network::Proxy;
      public:
        /// Create a configuration.
        ///
        /// \param timeout Initial value of the timeout attribute.
        /// \param version Initial value of the version attribute.
        Configuration(DurationOpt timeout = 30_sec,
                      DurationOpt stall_timeout = DurationOpt(),
                      Version version = Version::v11,
                      bool keep_alive = true,
                      boost::optional<Proxy> proxy = boost::optional<Proxy>());
        /// Configuration are not copiable.
        Configuration(Configuration const&) = default;
        /// Move a configuration.
        Configuration(Configuration&&) = default;
        /// Dispose of a Configuration.
        ~Configuration();
        /// Add a custom header to be sent to the server.
        ///
        /// \param name    Name of the header.
        /// \param content Content of the header.
        void
        header_add(std::string const& header, std::string const& content);
        /// Remove a header previously set with .
        ///
        /// \param name Name of the header to remove.
        void
        header_remove(std::string const& name);

      /*--------.
      | Cookies |
      `--------*/
      public:
        typedef std::unordered_map<std::string, std::string> Cookies;
        ELLE_ATTRIBUTE_RWX(Cookies, cookies);

      public:
        /// The HTTP version.
        ELLE_ATTRIBUTE_RW(Version, version);
        // If keep-alive should be enabled or not.
        ELLE_ATTRIBUTE_R(bool, keep_alive);
        /// The proxy to use.
        ELLE_ATTRIBUTE_RW(boost::optional<Proxy>, proxy);
        /// The potential request timeout.
        ELLE_ATTRIBUTE_RW(DurationOpt, timeout);
        /// Abort the request if both upload/download stall for given duration.
        ELLE_ATTRIBUTE_RW(DurationOpt, stall_timeout);
        typedef std::unordered_map<std::string, std::string> Headers;
        /// The additional HTTP headers to include in the request.
        ELLE_ATTRIBUTE_R(Headers, headers);
        /// Whether to used HTTP 1.1 chunked transfers.
        ///
        /// False by default. This enable to not send a Content-Length header
        /// and stream the outgoing body without knowing its total size
        /// beforehand. Unfortunately it is not supported:
        ///
        /// * by HTTP/1.0 and wsgiref (our development server)
        /// * by NGinx versions before 1.3, and Debian stable currently has 1.2.
        ///
        /// For those reasons and to stay on the safe side, it is disabled by
        /// default.  AFAICT we never upload massive bodies anyway, but this
        /// should be looked upon at some point.
        ELLE_ATTRIBUTE_RW(bool, chunked_transfers);
        /// The HTTP status to expect. Any different status will throw an
        /// exception upon waiting.
        ELLE_ATTRIBUTE_RW(boost::optional<StatusCode>, expected_status);

      /*----.
      | SSL |
      `----*/
      public:
        /// Whether to check if the FQDN of the server matches the one stated in
        /// his certificate.
        ELLE_ATTRIBUTE_RW(bool, ssl_verify_host);
      };


    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a request with no body.
      ///
      /// The request will start as soon as possible. It will not send a body
      /// and using the output stream interface will yield an error.
      ///
      /// \param url The URL to query.  \param method The HTTP method to use.
      /// \param conf The Configuration to apply.
      ///
      /// \throw RequestError if the request cannot be created and started.
      Request(std::string const& url,
              Method method = Method::GET,
              Configuration conf = Configuration());
      /// Create a request with a body.
      ///
      /// The request will start as soon as possible. It will send the body and
      /// fed trough the output stream interface and whose end is signaled by a
      /// call to the finalize() method.
      ///
      /// \param url          The URL to query.
      /// \param method       The HTTP method to use.
      /// \param content_type The MIME type of the uploded body.
      /// \param conf         The Configuration to apply.
      ///
      /// \throw RequestError if the request cannot be created and started.
      Request(std::string const& url,
              Method method,
              std::string const& content_type,
              Configuration conf = Configuration());
      /// Request are not copiable.
      Request(Request const& source) = delete;
      /// Move a Request.
      Request(Request&& source);
      /// Dispose of a request.
      ~Request();
      /// The HTTP method of this Request.
      ELLE_ATTRIBUTE_R(Method, method);
      /// The URL queried by this Request.
      ELLE_ATTRIBUTE_R(std::string, url);
    private:
      /// Helper to factor constructors.
      Request(std::string const& url,
              Method method,
              Configuration conf,
              bool flag);
      /// The actual implementation content.
      class Impl;
      /// The actual implementation.
      Impl* _impl;
      /// Let service manage our details.
      friend class Service;
      /// Let client manage our details.
      friend class Client;

    /*-------------.
    | Query String |
    `-------------*/
    public:
      /// Set the query string of the request.
      ///
      /// This should be done after construction, before the request is actually
      /// run.
      typedef std::unordered_map<std::string, std::string> QueryDict;
      ELLE_ATTRIBUTE_r(std::string, query_string);
      void
      query_string(QueryDict const& query_dict);

    /*-----------.
    | Completion |
    `-----------*/
    public:
      /// Signal end of the outgoing body.
      ///
      /// When streaming a body, this signal the request it will receive no more
      /// data and thus can inform the server all data was sent and expect its
      /// response. If forgotten, both the client and the server will stall in
      /// wait for potential more data.
      ///
      /// The Request is automatically finalized when waited (in the Waitable
      /// sense of the term) or when response is used.
      void
      finalize();
    private:
      /// Signal end of the server response (called by the Service).
      void
      _complete(int code);
      /// Wait for the request to be done.
      virtual
      bool
      _wait(Thread* thread, Waker const& waker) override;

    /*-------.
    | Status |
    `-------*/
    public:
      /// The response body.
      ///
      /// Getting the response will finalize() the request if needed and read
      /// all the server response in a buffer. The stream interface should be
      /// prefered most of the time to avoid buffering the whole response in
      /// memory and start handling it before it is entirerly downloaded. This
      /// is only suitable for short, simple server answers.
      elle::Buffer
      response();
      /// The HTTP status. Null until the request is completed.
      ELLE_ATTRIBUTE_r(StatusCode, status);
      /// How many time the request was paused in wait for output data.
      ELLE_attribute_r(int, pause_count);

    /*--------.
    | Headers |
    `--------*/
    public:
      /// The response headers.
      ///
      /// Headers are filled on the fly once the request has been started.
      ELLE_ATTRIBUTE_R(Configuration::Headers, headers);

    /*--------.
    | Cookies |
    `--------*/
    public:
      /// Get the query cookies.
      Request::Configuration::Cookies
      cookies() const;

    /*---------.
    | Progress |
    `---------*/
    public:
      struct Progress
      {
        bool
        operator == (Progress const& b) const;
        // Do not reorder, there are initializer list constructors out there !
        int64_t download_current;
        int64_t download_total;
        int64_t upload_current;
        int64_t upload_total; // Set to -1 if total unknown.
      };
      Progress
      progress() const;
      boost::signals2::signal<void (Progress const&)>&
      progress_changed();
      boost::signals2::signal<void (Progress const&)> const&
      progress_changed() const;

    /*----------.
    | Printable |
    `----------*/
    public:
      /// Pretty print the request.
      virtual
      void
      print(std::ostream& stream) const override;
    };

    std::ostream&
    operator <<(std::ostream& output,
                Request::Progress const& progress);

    /*----------.
    | Shortcuts |
    `----------*/

    /// Shortcut to perform a GET request and retreive response.
    ///
    /// This function stores the whole response in memory and should only be
    /// used for short, simple servers answers or when no body is expected.
    ///
    /// \param url  The URL to query.
    /// \param conf The Configuration for the request.
    /// \return The server response body.
    elle::Buffer
    get(std::string const& url,
        Request::Configuration conf = Request::Configuration());
  }
}

#endif
