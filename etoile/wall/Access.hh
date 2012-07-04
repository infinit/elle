#ifndef ETOILE_WALL_ACCESS_HH
# define ETOILE_WALL_ACCESS_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace wall
  {

    ///
    /// this class provides functionalities for manipulating the access
    /// control mechanism.
    ///
    class Access
    {
    public:
      //
      // static methods
      //
      static elle::Status       Lookup(const gear::Identifier&,
                                       const nucleus::neutron::Subject&,
                                       nucleus::neutron::Record*&);
      static elle::Status       Consult(const gear::Identifier&,
                                        const nucleus::neutron::Index&,
                                        const nucleus::neutron::Size&,
                                        nucleus::neutron::Range<
                                          nucleus::neutron::Record>&);
      static elle::Status       Grant(const gear::Identifier&,
                                      const nucleus::neutron::Subject&,
                                      const nucleus::neutron::Permissions&);
      static elle::Status       Revoke(const gear::Identifier&,
                                       const nucleus::neutron::Subject&);
    };

  }
}

#endif
