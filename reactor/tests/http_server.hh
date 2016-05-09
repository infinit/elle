#ifndef REACTOR_HTTP_SERVER_HH
# define REACTOR_HTTP_SERVER_HH

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
# include <reactor/thread.hh>

namespace reactor
{
  namespace http
  {
    namespace tests
    {
      class Server
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
        Server()
          : _server()
          , _port(0)
          , _accepter()
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          this->_server.listen(0);
          this->_port = this->_server.port();
          ELLE_TRACE_SCOPE("%s: listen on port %s", *this, this->_port);
          this->_accepter.reset(
            new reactor::Thread(*reactor::Scheduler::scheduler(),
                                "accepter",
                                std::bind(&Server::_accept,
                                          std::ref(*this))));
        }

        ~Server()
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          ELLE_TRACE_SCOPE("%s: destruction", *this);
          this->_finalize();
        }

        void
        _finalize()
        {
          this->_accepter->terminate_now();
        }

        std::string
        url(std::string const& path)
        {
          return elle::sprintf("http://127.0.0.1:%s/%s", this->port(), path);
        }


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
        ELLE_ATTRIBUTE(reactor::network::TCPServer, server);
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
          CommandLine(elle::Buffer const& buffer)
            : _path()
            , _method()
            , _version()
          {
            ELLE_LOG_COMPONENT("reactor.test.http");
            std::vector<std::string> words;
            boost::algorithm::split(words, buffer,
                                    boost::algorithm::is_any_of(" "));
            if (words.size() != 3)
              throw Server::Exception(
                elle::join(words.begin(), words.end(), " "),
                StatusCode::Bad_Request,
                "request command line sould have 3 members");

            std::vector<std::string> path_and_args;
            boost::algorithm::split(path_and_args, words[1],
                                    boost::algorithm::is_any_of("?"));

            this->_path = path_and_args[0];

            if (path_and_args.size() > 1)
            {
              std::vector<std::string> parameters;
              boost::algorithm::split(parameters, path_and_args[1],
                                      boost::algorithm::is_any_of("&"));
              ELLE_DEBUG("%s", parameters);
              for (auto const& param: parameters)
              {
                std::vector<std::string> key_value;
                ELLE_DEBUG("%s", key_value);
                boost::algorithm::split(key_value, param, boost::algorithm::is_any_of("="));
                this->_params[key_value[0]] = key_value[1];
              }
            }
            try
            {
              this->_method = reactor::http::method::from_string(words[0]);
              this->_version = reactor::http::version::from_string(words[2].substr(0, words[2].size() - 2));
            }
            catch (elle::Exception const& e)
            {
              throw Server::Exception(
                this->_path,
                reactor::http::StatusCode::Bad_Request,
                e.what());
            }
          }

          ELLE_ATTRIBUTE_R(std::string, path);
          ELLE_ATTRIBUTE(boost::optional<reactor::http::Method>, method);
          ELLE_ATTRIBUTE(boost::optional<reactor::http::Version>, version);
          ELLE_ATTRIBUTE_R(Parameters, params);

        public:
          Method const&
          method() const
          {
            if (this->_method)
              return this->_method.get();
            throw Server::Exception(
              this->_path, reactor::http::StatusCode::Bad_Request, "No method");
          }

          Version const&
          version() const
          {
            if (this->_version)
              return this->_version.get();
            throw Server::Exception(
              this->_path, reactor::http::StatusCode::Bad_Request, "No version");
          }

        public:
          virtual
          void
          print(std::ostream& output) const
          {
            elle::fprintf(output, "%s %s %s",
                         this->path(), this->params(), this->method());
          }
        };

        void
        _accept()
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
          {
            while (true)
            {
              std::unique_ptr<reactor::network::Socket> socket(
                this->_server.accept());
              ELLE_DEBUG("accept connection from %s", socket);
              auto name = elle::sprintf("request %s", socket);
              scope.run_background(
                name,
                std::bind(&Server::_serve, std::ref(*this),
                          elle::utility::move_on_copy(socket)));
            }
          };
        }

        virtual
        void
        _serve(std::unique_ptr<reactor::network::Socket> socket)
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          auto headers = this->_headers;
          Cookies cookies;
          try
          {
            CommandLine cmd(socket->read_until("\r\n"));
            ELLE_LOG_SCOPE("%s: handle request from %s: %s",
                           *this, socket, cmd);
            auto route = this->_routes.find(cmd.path());
            if (route == this->_routes.end())
            {
              ELLE_TRACE("%s: not found", *this);
              throw Exception(cmd.path(), reactor::http::StatusCode::Not_Found);
            }
            if (route->second.find(cmd.method()) == route->second.end())
            {
              ELLE_TRACE("%s: method not allowed", *this);
              throw Exception(cmd.path(),
                              reactor::http::StatusCode::Method_Not_Allowed);
            }
            while (true)
            {
              auto buffer = socket->read_until("\r\n");
              if (buffer == "\r\n")
                break;
              buffer.size(buffer.size() - 2);
              ELLE_TRACE("%s: get header: %s", *this, buffer.string());
              std::vector<std::string> words;
              boost::algorithm::split(words, buffer,
                                      boost::algorithm::is_any_of(" "));
              auto expected_length = [&] (unsigned int length) {
                if (words.size() != length)
                  throw Exception(cmd.path(),
                                  reactor::http::StatusCode::Bad_Request,
                                  elle::sprintf("%s: ill-formed", words));
                };
              if (words[0] == "Expect:")
              {
                expected_length(2);
                if (words[1] == "100-continue")
                  headers["Expect"] = "1";
              }
              else if (words[0] == "Content-Length:")
              {
                expected_length(2);
                headers["Content-Length"] = words[1];
              }
              else if (words[0] == "Content-Type:")
              {
                expected_length(2);
                headers["Content-Type"] = words[1];
              }
              else if (words[0] == "Transfer-Encoding:")
              {
                expected_length(2);
                if (words[1] == "chunked")
                  headers["chunked"] = "1";
              }
              else if (words[0] == "Set-Cookie:" || words[0] == "Cookie:")
              {
                std::vector<std::string> chunks;
                boost::algorithm::split(chunks, words[1],
                                        boost::algorithm::is_any_of("="));
                if (chunks.size() != 2)
                  throw Exception(cmd.path(),
                                  reactor::http::StatusCode::Bad_Request,
                                  elle::sprintf("%s: ill-formed", words));
                cookies[chunks[0]] = chunks[1];
              }
              else if (words[0] == "Connection:")
              {
                expected_length(2);
                headers["Connection"] = words[1];
              }
            }

            ELLE_TRACE("%s: cookies: %s", *this, cookies);
            ELLE_TRACE("%s: parameters: %s", *this, cmd.params());
            elle::Buffer content;
            if (cmd.version() == Version::v11 &&
                headers.find("Expect") != headers.end())
            {
              ELLE_TRACE("%s: send Continue header", *this)
              {
                std::string answer(
                  "HTTP/1.1 100 Continue\r\n"
                  "\r\n");
                socket->write(elle::ConstWeakBuffer(answer));
              }
            }
            if (headers.find("chunked") != headers.end())
              ELLE_TRACE("%s: read chunked content", *this)
                while (true)
                {
                  socket->read_until("\r\n"); // Ignore the chunked header.
                  auto buffer = socket->read_until("\r\n");
                  if (buffer == "\r\n")
                    break;
                  ELLE_DEBUG("%s: got content chunk from %s: %s",
                             *this, socket, buffer.string());
                  content.append(buffer.contents(), buffer.size() - 2);
                }
            else
            {
              auto content_length =
                boost::lexical_cast<unsigned int>(headers.at("Content-Length"));
              ELLE_TRACE("%s: read sized content", *this)
                content = this->read_sized_content(*socket, content_length);
            }
            ELLE_DUMP("%s: content: %s", *this, content);
            // Check JSON is valid. When getting meta_data on S3, we send a JSON
            // mimetype but an empty body, skip this case (and fix it later
            // cautiously).
            if (this->is_json(headers) && content.size() > 0)
            {
              try
              {
                elle::IOStream input(content.istreambuf());
                auto json = elle::json::read(input);
              }
              catch (elle::json::ParseError)
              {
                throw Exception(cmd.path(),
                                reactor::http::StatusCode::Bad_Request,
                                "invalid JSON");

              }
            }
            this->_response(
              *socket,
              StatusCode::OK,
              route->second.at(cmd.method())
                (headers, cookies, cmd.params(), content),
              cookies);
          }
          catch (Exception const& e)
          {
            ELLE_WARN("%s: http exception: %s", *this, e.what());
            this->_response(*socket, e.code(), this->is_json(headers) ? e.body() : e.what(), cookies);
          }
          catch (elle::Exception const& e)
          {
            ELLE_WARN("%s: internal error: %s", *this, e.what());
            this->_response(*socket, reactor::http::StatusCode::Internal_Server_Error, e.what(), cookies);
          }
          ELLE_TRACE("%s: close connection with %s", *this, socket);
        }

      public:
        // Register a fonction for a given route / method.
        virtual
        void
        register_route(std::string const& route,
                       Method method,
                       Function const& function)
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          ELLE_TRACE("%s: register %s on %s", *this, route, method);
          this->_routes[route][method] = function;
        }

        bool
        is_json(Headers const& headers) const
        {
          return (headers.find("Content-Type") != headers.end() &&
                  (headers.at("Content-Type") == "application/json"));
        }

      protected:
        virtual
        void
        _response(reactor::network::Socket& socket,
                  StatusCode code,
                  elle::ConstWeakBuffer content,
                  Cookies const& cookies = std::unordered_map<std::string,std::string>())
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          Headers headers = this->_headers;
          {
            std::string response;
            response += content.string();
            std::string answer = elle::sprintf(
              "HTTP/1.1 %s %s\r\n"
              "Server: Custom HTTP of doom\r\n",
              (int) code, code);
            headers["Content-Length"] = std::to_string(response.size());
            for (auto const& value: headers)
              answer += elle::sprintf("%s: %s\r\n", value.first, value.second);
            answer += "\r\n" + response;
            ELLE_TRACE("%s: send response to %s: %s %s",
                       *this, socket, static_cast<int>(code), code)
            {
              ELLE_DUMP("%s", answer);
              socket.write(elle::ConstWeakBuffer(answer));
            }
          }
        }

        elle::Buffer
        read_sized_content(reactor::network::Socket& socket, unsigned int length)
        {
          ELLE_LOG_COMPONENT("reactor.test.http");
          ELLE_TRACE_SCOPE("%s: read sized content of size %s", *this, length);
          elle::Buffer content(length);
          content.size(length);
          socket.read(
            reactor::network::Buffer(content.mutable_contents(), length));
          return content;
        }


        ELLE_ATTRIBUTE_RX(Headers, headers);

      /*----------.
      | Printable |
      `----------*/
      public:
        /// Pretty print the request.
        virtual
        void
        print(std::ostream& stream) const
        {
          stream << "HttpServer(port=" << this->_port << ")";
        }
      };
    }
  }
}

#endif
