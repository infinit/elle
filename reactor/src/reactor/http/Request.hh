#ifndef REACTOR_HTTP_REQUEST_HH
# define REACTOR_HTTP_REQUEST_HH

# include <string>

# include <boost/asio.hpp>

# include <elle/Buffer.hh>
# include <elle/IOStream.hh>
# include <elle/Printable.hh>
# include <elle/attribute.hh>

# include <reactor/operation.hh>

namespace reactor
{
  namespace http
  {
    /*-------.
    | Method |
    `-------*/

    enum class Method
    {
      DELETE,
      GET,
      POST,
      PUT,
    };

    std::ostream&
    operator <<(std::ostream& output,
                Method method);

    /*--------.
    | Version |
    `--------*/

    enum class Version
    {
      v10,
      v11,
    };

    std::ostream&
    operator <<(std::ostream& output,
                Version method);

    /*--------.
    | Request |
    `--------*/

    class Request:
      public reactor::Waitable,
      public elle::IOStream
    {
    /*--------------.
    | Configuration |
    `--------------*/
    public:
      class Configuration
      {
      public:
        Configuration(DurationOpt timeout = 30_sec,
                      Version version = Version::v11);
        Configuration(Configuration const& model) = default;
        Configuration(Configuration&& source) = default;
        ~Configuration();
        void
        header_add(std::string const& header, std::string const& content);
        void
        header_remove(std::string const& header);

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
        /// False by default.
        ELLE_ATTRIBUTE_RW(bool, chunked_transfers);
      };


    /*-------------.
    | Construction |
    `-------------*/
    public:
      Request(std::string const& url,
              Method method = Method::GET,
              Configuration conf = Configuration());
      Request(std::string const& url,
              Method method,
              std::string const& content_type,
              Configuration conf = Configuration());
      /// Request are not copiable.
      Request(Request const& source) = delete;
      /// Move a Request.
      Request(Request&& source);
      ~Request();
      boost::asio::ip::tcp::socket*
      socket();
    private:
      Request(std::string const& url,
              Method method,
              Configuration conf,
              bool flag);
      ELLE_ATTRIBUTE_R(Method, method);
      ELLE_ATTRIBUTE_R(std::string, url);
      class Impl;
      Impl* _impl;

    /*-----------.
    | Completion |
    `-----------*/
    public:
      /// Signal end of input.
      void
      finalize();
    private:
      friend class Service;
      void
      _complete(int code);
      virtual
      bool
      _wait(Thread* thread) override;

    /*-------.
    | Status |
    `-------*/
    public:
      /// The response body.
      elle::Buffer
      response();
      /// The HTTP status. Null until the request is completed.
      ELLE_ATTRIBUTE_R(int, status);
      /// Whether the request is currently uploading data.
      ELLE_ATTRIBUTE_R(bool, reading);
      /// Whether the request is currently downloading data.
      ELLE_ATTRIBUTE_R(bool, writing);

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const;
    };

    /*----------.
    | Shortcuts |
    `----------*/

    elle::Buffer
    get(std::string const& url,
        Request::Configuration conf = Request::Configuration());

    // elle::Buffer
    // post(std::string const& url,
    //      std::string const& content_type,
    //      std::istream& input,
    //      Request::Configuration conf = Request::Configuration());
  }
}

#endif
