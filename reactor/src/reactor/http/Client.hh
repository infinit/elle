#ifndef REACTOR_HTTP_CLIENT_HH
# define REACTOR_HTTP_CLIENT_HH

# include <string>

# include <reactor/http/Request.hh>

namespace reactor
{
  namespace http
  {
    class Client
    {
    public:
      Client();
      ~Client();
      template <typename ... Args>
      Request
      request(Args&& ... args);
      elle::Buffer
      get(std::string const& url,
          Request::Configuration conf = Request::Configuration());

    private:
      void
      _register(Request const& request);
      class Impl;
      Impl* _impl;
    };
  }
}

# include <reactor/http/Client.hxx>

#endif
