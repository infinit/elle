#ifndef COPY_COPY_HH
# define COPY_COPY_HH

# include <elle/types.hh>

# include <reactor/network/fwd.hh>

# include <etoile/path/fwd.hh>
# include <etoile/portal/Manifest.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Permissions.hh>

# include <protocol/fwd.hh>

# include <Infinit.hh>

namespace satellite
{

  ///
  /// this class implements the copy satellite.
  ///
  class Copy
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationFrom,
        OperationTo
      };

    /// Connect and authenticate to Etoile.
    static
    void
    connect();
    /// Copy a file from Infinit to the local file system.
    static
    void
    from(elle::String const& to,
         elle::String const& from);
    /// Copy the local file to Infinit.
    static
    void
    to(elle::String const& from,
       elle::String const& to);

    static reactor::network::TCPSocket* socket;
    static infinit::protocol::Serializer* serializer;
    static infinit::protocol::ChanneledStream* channels;
    static etoile::portal::RPC* rpcs;
  };

}

#endif
