#ifndef REACTOR_HTTP_CLIENT_HH
# define REACTOR_HTTP_CLIENT_HH

# include <string>

# include <reactor/http/Request.hh>

namespace reactor
{
  namespace http
  {
    /// HTTP client to run multiple requests in the same context.
    ///
    /// For now the context just includes the cookie jar.
    class Client
    {
    public:
      /// Create a Client.
      Client();
      /// Dispose of a Client.
      ~Client();
      /// Create a request and register it with this client.
      ///
      /// The request is created with the exact arguments passed and will use
      /// this client's context.
      ///
      /// \return The Request.
      template <typename ... Args>
      Request
      request(Args&& ... args);
      /// Perform a GET request in this client's context.
      ///
      /// This is a convenience shortcut to perform simplest request without
      /// creating a Request object.
      ///
      /// \return The server response.
      elle::Buffer
      get(std::string const& url,
          Request::Configuration conf = Request::Configuration());

    /*--------.
    | Cookies |
    `--------*/
    public:
      /// Get the client cookies.
      Request::Configuration::Cookies
      cookies() const;

    private:
      /// Register a Request to use this client's context.
      void
      _register(Request const& request);
      /// The actual implementation content.
      class Impl;
      /// The actual implementation.
      Impl* _impl;
    };
  }
}

# include <reactor/http/Client.hxx>

#endif
