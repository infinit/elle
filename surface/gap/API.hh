#ifndef  SURFACE_GAP_API_HH
# define SURFACE_GAP_API_HH

# include <memory>
# include <string>

# include <elle/format/json/Object.hh>

namespace surface
{
  namespace gap
  {

    class API
    {
    public:
      typedef std::unique_ptr<elle::format::json::Object> ObjectPtr;
    private:
      std::string _server;
      uint16_t    _port;
      std::string _token;

    public:
      API(std::string const& server = "localhost", uint16_t port = 12345);
    public:
      ObjectPtr post(std::string const& url,
                     elle::format::json::Object const& params);

    };

  }
}

#endif

