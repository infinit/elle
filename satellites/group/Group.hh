#ifndef GROUP_GROUP_HH
# define GROUP_GROUP_HH

# include <elle/types.hh>
# include <elle/network/fwd.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <Infinit.hh>

namespace satellite
{

  ///
  /// this class implements the group satellite.
  ///
  class Group
  {
  public:
    //
    // enumerations
    //
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

    //
    // static methods
    //
    // XXX
    static
    elle::Status
    Connect();
    /// XXX
    static
    elle::Status
    Display(nucleus::neutron::Fellow const& fellow);
    // XXX
    static
    elle::Status
    Information(typename nucleus::neutron::Group::Identity const& identity);
    /// XXX
    static
    elle::Status
    Create(elle::String const& description);
    /// XXX
    static
    elle::Status
    Add(typename nucleus::neutron::Group::Identity const& identity,
        nucleus::neutron::Subject const& subject);
    /// XXX
    static
    elle::Status
    Lookup(typename nucleus::neutron::Group::Identity const& identity,
           nucleus::neutron::Subject const& subject);
    /// XXX
    static
    elle::Status
    Consult(typename nucleus::neutron::Group::Identity const& identity);
    /// XXX
    static
    elle::Status
    Remove(typename nucleus::neutron::Group::Identity const& identity,
           nucleus::neutron::Subject const& subject);
    /// XXX
    static
    elle::Status
    Destroy(typename nucleus::neutron::Group::Identity const& identity);

    //
    // static attributes
    //
    static elle::network::TCPSocket* socket;
  };

}

#endif
