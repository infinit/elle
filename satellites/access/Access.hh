#ifndef ACCESS_ACCESS_HH
# define ACCESS_ACCESS_HH

# include <elle/types.hh>
# include <elle/network/fwd.hh>

# include <etoile/path/fwd.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Permissions.hh>

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

    //
    // static methods
    //
    static elle::Status         Connect();

    static elle::Status
    Display(nucleus::neutron::Record const&);

    static elle::Status         Lookup(const etoile::path::Way&,
                                       const nucleus::neutron::Subject&);
    static elle::Status         Consult(const etoile::path::Way&);
    static elle::Status         Grant(const etoile::path::Way&,
                                      const nucleus::neutron::Subject&,
                                      const nucleus::neutron::Permissions);
    static elle::Status         Revoke(const etoile::path::Way&,
                                       const nucleus::neutron::Subject&);

    //
    // static attributes
    //
    static elle::network::TCPSocket* socket;
  };

}

#endif
