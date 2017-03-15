#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include <elle/Buffer.hh>
#include <elle/With.hh>
#include <elle/attribute.hh>
#include <elle/json/exceptions.hh>
#include <elle/json/json.hh>
#include <elle/log.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/exception.hh>
#include <elle/reactor/http/Method.hh>
#include <elle/reactor/http/StatusCode.hh>
#include <elle/reactor/http/Version.hh>
#include <elle/reactor/network/socket.hh>
#include <elle/reactor/network/tcp-server.hh>
#include <elle/reactor/network/tcp-socket.hh>
#include <elle/reactor/Thread.hh>
#include <elle/utility/Move.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A reactorfied HTTP server.
      ///
      /// N.B. This is not a fully compliant HTTP Server.
      ///
      /// \code{.cc}
      ///
      /// HTTPServer server;
      /// server.register_route(
      ///   "/",
      ///   elle::reactor::http::Method::GET,
      ///   [] (HTTPServer::Headers const&,
      ///       HTTPServer::Cookies const&,
      ///       HTTPServer::Parameters const&,
      ///       elle::Buffer const&) -> std::string
      ///   {
      ///     return "Hello you!";
      ///   });
      ///
      /// elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
      /// {
      ///   s.run_background("server", [&s] { server.run(); });
      ///   s.run_background("get",
      ///     [&]
      ///     {
      ///       elle::reactor::http::Request r(
      ///         elle::sprintf("127.0.0.1:%s/", s.port()));
      ///       std::cout << r.string();
      ///     });
      ///   s.wait();
      /// };
      ///
      /// // Result: "Hello you!";
      ///
      /// \endcode
      class HttpServer
        : public elle::Printable
      {
      public:
        // Use internally.
        class Exception:
          public elle::Error
        {
        public:
          using Super = elle::Error;
          Exception(std::string const& url,
                    reactor::http::StatusCode code,
                    std::string const& body = "")
            : Super(
              elle::sprintf("error requesting %s: %s (%s)", url, code, body))
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
        /// Create an HTTPServer to listen on a specific port.
        ///
        /// \param port The port to listen on.
        HttpServer(int port);
        virtual
        ~HttpServer();
        HttpServer(HttpServer&&) = delete;
        HttpServer(HttpServer const&) = delete;
      protected:
        void
        _finalize();
      public:
        /// Helper to get the local HTTP route for a given path.
        ///
        /// \code{.cc}
        ///
        /// elle::reactor::http::HttpServer s(4020);
        /// s.path("foo/bar"); // http://127.0.0.1:4020/foo/bar.
        ///
        /// \endcode
        ///
        /// \param path The path of the URL.
        /// \returns Complete URL to given path.
        std::string
        url(std::string const& path);

        using Headers = std::unordered_map<std::string, std::string>;
        using Cookies = std::unordered_map<std::string, std::string>;
        using Parameters = std::unordered_map<std::string, std::string>;
        using Function = std::function<std::string (Headers const&,
                                                    Cookies const&,
                                                    Parameters const&,
                                                    elle::Buffer const&)>;
        // Matching a Method to a specific function.
        // e.g.: {GET -> get_function, POST -> post_function, ...}
        struct enum_hash
        {
          template <typename T>
          inline
          std::enable_if_t<std::is_enum<T>::value, std::size_t>
          operator ()(T const value) const
          {
            return static_cast<std::size_t>(value);
          }
        };
        // XXX: Find better names.
        using MethodFunctions =
          std::unordered_map<reactor::http::Method, Function, enum_hash>;
        // Matching a route a MethodFunctions.
        // e.g.: {"/foo" -> {GET -> get_function, POST -> post_function, ...}}
        using Routes = std::unordered_map<std::string, MethodFunctions>;
        ELLE_ATTRIBUTE_X(Routes, routes);
        ELLE_ATTRIBUTE(std::unique_ptr<reactor::network::Server>, server);
        ELLE_ATTRIBUTE_R(int, port);
        ELLE_ATTRIBUTE(std::unique_ptr<reactor::Thread>, accepter);
        ELLE_ATTRIBUTE_RW(std::function<void (std::string const&)>,
                          check_version);
        ELLE_ATTRIBUTE_RW(std::function<void (std::string const&)>,
                          check_method);
        ELLE_ATTRIBUTE_RW(std::function<void (bool)>, check_expect_continue);
        ELLE_ATTRIBUTE_RW(std::function<void (bool)>, check_chunked);

      private:
        /// Extract method, path and version for the HTTP headers.
        struct CommandLine
          : public elle::Printable
        {
          CommandLine(elle::Buffer const& buffer);
          /// Path requested.
          ELLE_ATTRIBUTE_R(std::string, path);
          /// Method used.
          ELLE_ATTRIBUTE(boost::optional<reactor::http::Method>, method);
          /// HTTP version used.
          ELLE_ATTRIBUTE(boost::optional<reactor::http::Version>, version);
          /// URL parameters used.
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
        /// Register a function to a pair (route / method).
        ///
        /// \param route The route.
        /// \param method The Method.
        /// \param function The Function to call.
        virtual
        void
        register_route(std::string const& route,
                       http::Method method,
                       Function const& function);
        /// Check if content-type is application/json.
        ///
        /// \param headers The headers of the Request.
        /// \returns Whether the request is JSON or not.
        bool
        is_json(Headers const& headers) const;
      protected:
        virtual
        void
        _response(reactor::network::Socket& socket,
                  http::StatusCode code,
                  elle::ConstWeakBuffer content,
                  Cookies const& cookies = Cookies{});
        elle::Buffer
        read_sized_content(reactor::network::Socket& socket,
                           unsigned int length);
        ELLE_ATTRIBUTE_RX(Headers, headers);
      public:
        virtual
        void
        print(std::ostream& stream) const;
      };
    }
  }
}
