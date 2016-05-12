#include <reactor/network/http-server.hh>


namespace reactor
{
  namespace network
  {
    HttpServer::HttpServer(std::unique_ptr<Server> server)
    : _server(std::move(server))
    , _port(0)
    , _accepter()
    {
      ELLE_LOG_COMPONENT("reactor.test.http");
      if (!this->_server)
      {
        auto server = elle::make_unique<TCPServer>();
        server->listen();
        this->_port = server->port();
        this->_server = std::move(server);
        ELLE_TRACE_SCOPE("%s: listen on port %s", *this, this->_port);
      }
      this->_accepter.reset(
        new reactor::Thread(*reactor::Scheduler::scheduler(),
                            "accepter",
                            std::bind(&HttpServer::_accept,
                              std::ref(*this))));
    }
    HttpServer::~HttpServer()
    {
      ELLE_LOG_COMPONENT("reactor.test.http");
      ELLE_TRACE_SCOPE("%s: destruction", *this);
      this->_finalize();
    }
    void
    HttpServer::_finalize()
    {
      this->_accepter->terminate_now();
    }

    std::string
    HttpServer::url(std::string const& path)
    {
      return elle::sprintf("http://127.0.0.1:%s/%s", this->port(), path);
    }

    HttpServer::CommandLine::CommandLine(elle::Buffer const& buffer)
    : _path()
    , _method()
    , _version()
    {
       ELLE_LOG_COMPONENT("reactor.test.http");
       std::vector<std::string> words;
       boost::algorithm::split(words, buffer,
                               boost::algorithm::is_any_of(" "));
       if (words.size() != 3)
         throw HttpServer::Exception(
           elle::join(words.begin(), words.end(), " "),
           http::StatusCode::Bad_Request,
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
         throw HttpServer::Exception(
           this->_path,
           reactor::http::StatusCode::Bad_Request,
           e.what());
       }
    }
    http::Method const&
    HttpServer::CommandLine::method() const
    {
      if (this->_method)
        return this->_method.get();
      throw HttpServer::Exception(
        this->_path, reactor::http::StatusCode::Bad_Request, "No method");
    }

    http::Version const&
    HttpServer::CommandLine::version() const
    {
      if (this->_version)
        return this->_version.get();
      throw HttpServer::Exception(
        this->_path, reactor::http::StatusCode::Bad_Request, "No version");
    }
    void
    HttpServer::CommandLine::print(std::ostream& output) const
    {
      elle::fprintf(output, "%s %s %s",
                    this->path(), this->params(), this->method());
    }

    void
    HttpServer::_accept()
    {
      ELLE_LOG_COMPONENT("reactor.test.http");
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        while (true)
        {
          auto socket = elle::utility::move_on_copy(this->_server->accept());
          ELLE_DEBUG("accept connection from %s", socket);
          auto name = elle::sprintf("request %s", socket);
          scope.run_background(
            name,
            [this, socket]
            {
              try
              {
                std::unique_ptr<reactor::network::Socket> s =
                  std::move(*socket);
                this->_serve(std::move(s));
              }
              catch (...)
              {
                ELLE_ERR("%s: fatal error serving client: %s",
                         this, elle::exception_string());
                throw;
              }
            });
        }
      };
    }

    void
    HttpServer::_serve(std::unique_ptr<reactor::network::Socket> socket)
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
        if (cmd.version() == http::Version::v11 &&
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
          http::StatusCode::OK,
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
    void
    HttpServer::register_route(std::string const& route,
                               http::Method method,
                               Function const& function)
    {
      ELLE_LOG_COMPONENT("reactor.test.http");
      ELLE_TRACE("%s: register %s on %s", *this, route, method);
      this->_routes[route][method] = function;
    }
    bool
    HttpServer::is_json(Headers const& headers) const
    {
      return (headers.find("Content-Type") != headers.end() &&
        (headers.at("Content-Type") == "application/json"));
    }
    void
    HttpServer::_response(reactor::network::Socket& socket,
                  http::StatusCode code,
                  elle::ConstWeakBuffer content,
                  Cookies const& cookies)
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
    HttpServer::read_sized_content(reactor::network::Socket& socket, unsigned int length)
    {
      ELLE_LOG_COMPONENT("reactor.test.http");
      ELLE_TRACE_SCOPE("%s: read sized content of size %s", *this, length);
      elle::Buffer content(length);
      content.size(length);
      socket.read(
        reactor::network::Buffer(content.mutable_contents(), length));
      return content;
    }
    void
    HttpServer::print(std::ostream& stream) const
    {
      stream << "HttpServer(port=" << this->_port << ")";
    }
  }
}
