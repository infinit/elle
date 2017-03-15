#pragma once

#include <memory>

#include <elle/reactor/asio.hh>
#include <elle/reactor/network/fwd.hh>
#include <elle/reactor/network/Protocol.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// Abstract version Server that can accept connections and provide
      /// Sockets.
      class Server
        : public elle::Printable
      {
      /*------.
      | Types |
      `------*/
      public:
        using Self = Server;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Create a server.
        ///
        /// \param sched The underlying Scheduler.
        Server(Scheduler& scheduler);
        /// Create a server using the current Scheduler.
        Server();
        virtual
        ~Server();

      /*----------.
      | Accepting |
      `----------*/
      public:
        /// Return a Socket by calling _accept.
        std::unique_ptr<Socket>
        accept();
      protected:
        virtual
        std::unique_ptr<Socket>
        _accept() = 0;
        ELLE_ATTRIBUTE(Scheduler&, scheduler, protected);
      };

      /// Abstract specialization of a Server for
      /// - A specific type of Socket
      /// - A specific type of Endpoint
      /// - A specific type of Acceptor.
      template <typename AsioSocket_, typename EndPoint_, typename Acceptor_>
      class ProtoServer
        : public Server
      {
      /*------.
      | Types |
      `------*/
      public:
        using AsioSocket = AsioSocket_;
        using Acceptor = Acceptor_;
        using EndPoint = EndPoint_;

      /*----------.
      | Accepting |
      `----------*/
      protected:
        using Server::_accept;

      /*----------.
      | Listening |
      `----------*/
      public:
        /// Reset the acceptor with a new instance for the given endpoint.
        ///
        /// \param endpoint Endpoint to listen to.
        void
        listen(EndPoint const& endpoint);
        /// Reset the acceptor with a new instance for the default endpoint.
        void
        listen();
        /// Return our acceptor's local_endpoint.
        EndPoint
        local_endpoint() const;
      protected:
        void
        _accept(AsioSocket& socket, EndPoint& peer);
        virtual
        EndPoint
        _default_endpoint() const = 0;
        ELLE_ATTRIBUTE_X(std::unique_ptr<Acceptor>, acceptor);

      /*----------.
      | Printable |
      `----------*/
      public:
        virtual
        void
        print(std::ostream& stream) const override;
      };
    }
  }
}
