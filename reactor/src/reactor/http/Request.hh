#ifndef REACTOR_HTTP_REQUEST_HH
# define REACTOR_HTTP_REQUEST_HH

# include <string>

# include <boost/asio.hpp>

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <reactor/http/fwd.hh>
# include <reactor/operation.hh>

namespace reactor
{
  namespace http
  {
    /*-------.
    | Method |
    `-------*/

    /// HTTP methods used to perform resquests.
    enum class Method
    {
      DELETE,
      GET,
      POST,
      PUT,
    };

    /// Pretty print a HTTP method.
    std::ostream&
    operator <<(std::ostream& output,
                Method method);

    /*--------.
    | Version |
    `--------*/

    /// HTTP protocol versions.
    enum class Version
    {
      v10,
      v11,
    };

    /// Pretty print a HTTP version.
    std::ostream&
    operator <<(std::ostream& output,
                Version method);

    /*-----------.
    | StatusCode |
    `-----------*/

    /// HTTP status code
    enum class StatusCode
    {
      Continue = 100,
      Switching_Protocols = 101,
      OK = 200,
      Created = 201,
      Accepted = 202,
      Non_Authoritative_Information = 203,
      No_Content = 204,
      Reset_Content = 205,
      Partial_Content = 206,
      Multiple_Choices = 300,
      Moved_Permanently = 301,
      Found = 302,
      See_Other = 303,
      Not_Modified = 304,
      Use_Proxy = 305,
      Unused = 306,
      Temporary_Redirect = 307,
      Bad_Request = 400,
      Unauthorized = 401,
      Payment_Required = 402,
      Forbidden = 403,
      Not_Found = 404,
      Method_Not_Allowed = 405,
      Not_Acceptable = 406,
      Proxy_Authentication_Required = 407,
      Request_Timeout = 408,
      Conflict = 409,
      Gone = 410,
      Length_Required = 411,
      Precondition_Failed = 412,
      Request_Entity_Too_Large = 413,
      Request_URI_Too_Long = 414,
      Unsupported_Media_Type = 415,
      Requested_Range_Not_Satisfiable = 416,
      Expectation_Failed = 417,
      Internal_Server_Error = 500,
      Not_Implemented = 501,
      Bad_Gateway = 502,
      Service_Unavailable = 503,
      Gateway_Timeout = 504,
      HTTP_Version_Not_Supported = 505,
    };

    /// Pretty print a HTTP status code.
    std::ostream&
    operator <<(std::ostream& output,
                StatusCode method);

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
      public:
        /// Create a configuration.
        ///
        /// \param timeout Initial value of the timeout attribute.
        /// \param version Initial value of the version attribute.
        Configuration(DurationOpt timeout = 30_sec,
                      Version version = Version::v11);
        /// Configuration are not copiable.
        Configuration(Configuration const& model) = default;
        /// Move a configuration.
        Configuration(Configuration&& source) = default;
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

      public:
        /// The HTTP version.
        ELLE_ATTRIBUTE_RW(Version, version);
        /// The potential request timeout.
        ELLE_ATTRIBUTE_RW(DurationOpt, timeout);
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
      /// The underlying ASIO socket performing the request, for Servic access.
      boost::asio::ip::tcp::socket*
      socket();
      /// The actual implementation content.
      class Impl;
      /// The actual implementation.
      Impl* _impl;
      /// Let service manage our details.
      friend class Service;
      /// Let client manage our details.
      friend class Client;

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
      _wait(Thread* thread) override;

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
      /// Whether the request is currently uploading data.
      ELLE_ATTRIBUTE_R(bool, reading);
      /// Whether the request is currently downloading data.
      ELLE_ATTRIBUTE_R(bool, writing);
      /// How many time the request was paused in wait for output data.
      ELLE_attribute_r(int, pause_count);

    /*----------.
    | Printable |
    `----------*/
    public:
      /// Pretty print the request.
      virtual
      void
      print(std::ostream& stream) const;
    };

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
