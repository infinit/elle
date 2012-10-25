#ifndef PROGRESS_PROGRESS_HH
# define PROGRESS_PROGRESS_HH

# include <elle/types.hh>

# include <reactor/network/fwd.hh>

# include <etoile/portal/Manifest.hh>

# include <protocol/fwd.hh>

# include <Infinit.hh>

namespace satellite
{

  ///
  /// this class implements the progress satellite.
  ///
  class Progress
  {
  public:
    /// Connect and authenticate to Etoile.
    static
    void
    connect();
    /// Retrieve and display the current progress.
    static
    void
    display();

    static reactor::network::TCPSocket* socket;
    static infinit::protocol::Serializer* serializer;
    static infinit::protocol::ChanneledStream* channels;
    static etoile::portal::RPC* rpcs;
  };

}

#endif
