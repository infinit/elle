#ifndef GROUP_GROUP_HH
# define GROUP_GROUP_HH

# include <elle/types.hh>

# include <reactor/network/fwd.hh>

# include <etoile/Manifest.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <protocol/fwd.hh>

# include <Infinit.hh>

namespace satellite
{
  /// The group satellite.
  class Group
  {
  public:
    enum Operation
      {
        OperationUnknown = 0,

        OperationInformation,
        OperationCreate,
        OperationAdd,
        OperationLookup,
        OperationConsult,
        OperationRemove,
        OperationDestroy
      };

    /// XXX
    static
    void
    connect();
    /// XXX
    static
    void
    display(nucleus::neutron::Fellow const& fellow);
    /// XXX
    static
    void
    information(typename nucleus::neutron::Group::Identity const& identity);
    /// XXX
    static
    void
    create(elle::String const& description);
    /// XXX
    static
    void
    add(typename nucleus::neutron::Group::Identity const& identity,
        nucleus::neutron::Subject const& subject);
    /// XXX
    static
    void
    lookup(typename nucleus::neutron::Group::Identity const& identity,
           nucleus::neutron::Subject const& subject);
    /// XXX
    static
    void
    consult(typename nucleus::neutron::Group::Identity const& identity);
    /// XXX
    static
    void
    remove(typename nucleus::neutron::Group::Identity const& identity,
           nucleus::neutron::Subject const& subject);
    /// XXX
    static
    void
    destroy(typename nucleus::neutron::Group::Identity const& identity);

    static reactor::network::TCPSocket* socket;
    static infinit::protocol::Serializer* serializer;
    static infinit::protocol::ChanneledStream* channels;
    static etoile::portal::RPC* rpcs;
  };
}

#endif
