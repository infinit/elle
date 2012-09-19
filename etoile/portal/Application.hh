#ifndef ETOILE_PORTAL_APPLICATION_HH
# define ETOILE_PORTAL_APPLICATION_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>
# include <elle/radix/Object.hh>

# include <reactor/network/fwd.hh>

# include <etoile/portal/Manifest.hh>

namespace etoile
{
  namespace portal
  {

    ///
    /// this class represents an external client triggering operations
    /// on Etoile.
    ///
    /// note that the client must authenticate to Etoile in order to
    /// use the message interface.
    ///
    class Application:
      public elle::radix::Entity,
      public elle::Printable
    {
    public:
      //
      // constants
      //
      static const reactor::Duration              Timeout;

      //
      // enumerations
      //
      enum State
        {
          StateConnected = 1,
          StateAuthenticated,
          StateDisconnected
        };

      enum Processing
        {
          ProcessingOff = 0,
          ProcessingOn
        };

      /*-------------.
      | Construction |
      `-------------*/
      Application();
      ~Application();

      //
      // methods
      //
      elle::Status      Create(reactor::network::TCPSocket*);

      //
      // callbacks
      //
      elle::Status      Abort();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      State                     state;
      Processing                processing;

      reactor::network::TCPSocket* socket;
      infinit::protocol::Serializer* serializer;
      infinit::protocol::ChanneledStream* channels;
      etoile::portal::RPC* rpcs;

    /*----------.
    | Printable |
    `----------*/
    public:
      virtual
      void
      print(std::ostream& stream) const;

    /*-------------.
    | RPC handlers |
    `-------------*/
    private:
      void _run();
      bool _authenticate(std::string const& pass);
    };
  }
}

#endif
