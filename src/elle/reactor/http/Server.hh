#pragma once

#include <elle/attribute.hh>

#include <elle/reactor/http/Method.hh>
#include <elle/reactor/http/StatusCode.hh>
#include <elle/reactor/network/TCPServer.hh>


namespace elle
{
  namespace reactor
  {
    namespace http
    {
      class Headers
      {
      public:
        Headers()
        {}

      };

      /// HTTP server.
      class Server
      {
      public:
        Server(int port = 0);

        /// Outgoing HTTP response.
        class Response
          : public IOStream
        {
        public:
          Response(Response&&);
          ~Response();
        private:
          struct Impl;
          ELLE_ATTRIBUTE(std::unique_ptr<Impl>, impl);
          friend class Server;
          friend class Body;
          Response(StatusCode status, std::unique_ptr<Impl> impl);
          ELLE_ATTRIBUTE_R(StatusCode, status);
        };

        /// Incoming HTTP request.
        class Request
          : public IOStream
          , public Headers
        {
        public:
          friend class Server;
          Request(Request&&);
          ~Request();
          Response
          respond(StatusCode status);
        private:
          struct Impl;
          ELLE_ATTRIBUTE(std::unique_ptr<Impl>, impl);
          Request(std::unique_ptr<Impl> impl);
          ELLE_ATTRIBUTE_RW(Method, method);
          ELLE_ATTRIBUTE_RW(std::string, uri);
        };

        void
        run();

        virtual
        void
        _handle(Request r) = 0;

        ELLE_attribute_r(int, port);
        ELLE_ATTRIBUTE(network::TCPServer, server);
      };
    }
  }
}
