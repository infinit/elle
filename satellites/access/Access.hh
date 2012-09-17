#ifndef ACCESS_ACCESS_HH
# define ACCESS_ACCESS_HH

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
  /// this class implements the access satellite.
  ///
  class Access
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationLookup,
        OperationConsult,
        OperationGrant,
        OperationRevoke
      };

    /// Connect and authenticate to Etoile.
    static void
    connect();

    /// Lookup the access record associated with the given identifier.
    static
    void
    lookup(etoile::path::Way const&,
           nucleus::neutron::Subject const&);
    /// Display all the access records for the given path.
    static
    void
    consult(etoile::path::Way const&);
    /// Grant access to the entity referenced by the given identifier.
    static
    void
    grant(etoile::path::Way const&,
          nucleus::neutron::Subject const&,
          nucleus::neutron::Permissions const);
    /// Revoke an existing access.
    static
    void
    revoke(etoile::path::Way const&,
           nucleus::neutron::Subject const&);

    static reactor::network::TCPSocket* socket;
    static infinit::protocol::Serializer* serializer;
    static infinit::protocol::ChanneledStream* channels;
    static etoile::portal::RPC* rpcs;
  };

}

#endif
