#ifndef ETOILE_PORTAL_APPLICATION_HH
# define ETOILE_PORTAL_APPLICATION_HH

# include <elle/types.hh>
# include <elle/network/LocalSocket.hh>

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
      public elle::radix::Entity
    {
    public:
      //
      // constants
      //
      static const elle::Natural32              Timeout;

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

      //
      // constructors & destructors
      //
      Application();
      ~Application();

      //
      // methods
      //
      elle::Status      Create(elle::network::LocalSocket*);

      //
      // callbacks
      //
      elle::Status      Disconnected();
      // XXX: const string&
      elle::Status      Error(elle::String);
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

      elle::concurrency::Timer*              timer;
      elle::network::LocalSocket*        socket;
    };

  }
}

#endif
