#pragma once

#include <elle/Buffer.hh>
#include <elle/attribute.hh>

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      class WebSocket
        // : public elle::IOStream
      {
      /*------.
      | Types |
      `------*/
      public:
        using Self = WebSocket;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        WebSocket(std::string const& uri);
        WebSocket(WebSocket&& source);
        ~WebSocket();

      /*--------------.
      | Communication |
      `--------------*/
      public:
        Buffer
        receive();
        void
        send(ConstWeakBuffer b);

      /*-----.
      | Impl |
      `-----*/
      private:
        class Impl;
        friend class WebSocketHandshake;
        friend class WebSocketRead;
        friend class WebSocketWrite;
        ELLE_ATTRIBUTE(std::unique_ptr<Impl>, impl);
      };
    }
  }
}
