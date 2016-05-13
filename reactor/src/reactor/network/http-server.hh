#ifndef REACTOR_NETWORK_HTTP_SERVER_HH
# define REACTOR_NETWORK_HTTP_SERVER_HH

# include <functional>
# include <memory>
# include <string>
# include <unordered_map>

# include <boost/algorithm/string.hpp>
# include <boost/lexical_cast.hpp>
# include <boost/optional.hpp>

# include <elle/Buffer.hh>
# include <elle/With.hh>
# include <elle/attribute.hh>
# include <elle/json/exceptions.hh>
# include <elle/json/json.hh>
# include <elle/log.hh>
# include <elle/string/algorithm.hh>
# include <elle/utility/Move.hh>

# include <reactor/Scope.hh>
# include <reactor/exception.hh>
# include <reactor/http/Method.hh>
# include <reactor/http/StatusCode.hh>
# include <reactor/http/Version.hh>
# include <reactor/network/buffer.hh>
# include <reactor/network/socket.hh>
# include <reactor/network/tcp-server.hh>
# include <reactor/network/tcp-socket.hh>
# include <reactor/thread.hh>

namespace reactor
{
  namespace network
  {
    class HttpServer
        : public elle::Printable
    {
      public:
        // Use internally.
        class Exception:
          public elle::Error
        {
        public:
          typedef elle::Error Super;
          Exception(std::string const& url,
                    reactor::http::StatusCode code,
                    std::string const& body = "")
            : Super(elle::sprintf("error requesting %s: %s (%s)", url, code, body))
            , _url(url)
            , _code(code)
            , _body(body)
          {}

        private:
          ELLE_ATTRIBUTE_R(std::string, url);
          ELLE_ATTRIBUTE_R(reactor::http::StatusCode, code);
          ELLE_ATTRIBUTE_R(std::string, body);
        };

      public:
        HttpServer(std::unique_ptr<Server> server = {});
        virtual
        ~HttpServer();
        HttpServer(HttpServer&&) = delete;
        HttpServer(HttpServer const&) = delete;
        void
        _finalize();
        std::string
        url(std::string const& path);

        typedef std::unordered_map<std::string, std::string> Headers;
        typedef std::unordered_map<std::string, std::string> Cookies;
        typedef std::unordered_map<std::string, std::string> Parameters;
        typedef std::function<std::string (Headers const&,
                                           Cookies const&,
                                           Parameters const&,
                                           elle::Buffer const&)>
        Function;
        // Matching a Method to a specific function.
        // e.g.: {GET -> get_function, POST -> post_function, ...}
        struct enum_hash
        {
          template <typename T>
          inline
          typename std::enable_if<std::is_enum<T>::value, std::size_t>::type
          operator ()(T const value) const
          {
            return static_cast<std::size_t>(value);
          }
        };
        // XXX: Find better names.
        typedef std::unordered_map<reactor::http::Method, Function, enum_hash> MethodFunctions;
        // Matching a route a MethodFunctions.
        // e.g.: {"/foo" -> {GET -> get_function, POST -> post_function, ...}}
        typedef std::unordered_map<std::string, MethodFunctions> Routes;
        ELLE_ATTRIBUTE_X(Routes, routes);
        ELLE_ATTRIBUTE(std::unique_ptr<reactor::network::Server>, server);
        ELLE_ATTRIBUTE_R(int, port);
        ELLE_ATTRIBUTE(std::unique_ptr<reactor::Thread>, accepter);
        ELLE_ATTRIBUTE_RW(std::function<void (std::string const&)>, check_version);
        ELLE_ATTRIBUTE_RW(std::function<void (std::string const&)>, check_method);
        ELLE_ATTRIBUTE_RW(std::function<void (bool)>, check_expect_continue);
        ELLE_ATTRIBUTE_RW(std::function<void (bool)>, check_chunked);
      private:
        // Wrapper around the command line.
        // Extract method, path and version.
        struct CommandLine
          : public elle::Printable
        {
          CommandLine(elle::Buffer const& buffer);
          ELLE_ATTRIBUTE_R(std::string, path);
          ELLE_ATTRIBUTE(boost::optional<reactor::http::Method>, method);
          ELLE_ATTRIBUTE(boost::optional<reactor::http::Version>, version);
          ELLE_ATTRIBUTE_R(Parameters, params);
        public:
          http::Method const&
          method() const;
          http::Version const&
          version() const;
          virtual
          void
          print(std::ostream& output) const;
        };
        void
        _accept();
        virtual
        void
        _serve(std::unique_ptr<reactor::network::Socket> socket);
      public:
        virtual
        void
        register_route(std::string const& route,
                       http::Method method,
                       Function const& function);
        bool
        is_json(Headers const& headers) const;
      protected:
        virtual
        void
        _response(reactor::network::Socket& socket,
                  http::StatusCode code,
                  elle::ConstWeakBuffer content,
                  Cookies const& cookies = std::unordered_map<std::string,std::string>());
        elle::Buffer
        read_sized_content(reactor::network::Socket& socket, unsigned int length);
        ELLE_ATTRIBUTE_RX(Headers, headers);
      public:
        virtual
        void
        print(std::ostream& stream) const;
    };
  }
}


#endif
