#ifndef ETOILE_AUTOMATON_ACCESS_HH
# define ETOILE_AUTOMATON_ACCESS_HH

# include <elle/types.hh>
# include <elle/cryptography/fwd.hh>
# include <nucleus/neutron/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides a set of automata for manipulating the
    /// access control list embedded in the Access block referenced by
    /// the object.
    ///
    class Access
    {
    public:
      //
      // static methods
      //
      static elle::Status       Open(gear::Object&);

      static elle::Status       Grant(gear::Object&,
                                      const nucleus::neutron::Subject&,
                                      const nucleus::neutron::Permissions&);
      static elle::Status       Lookup(gear::Object&,
                                       const nucleus::neutron::Subject&,
                                       nucleus::neutron::Record*&);
      static elle::Status       Consult(gear::Object&,
                                        const nucleus::neutron::Index&,
                                        const nucleus::neutron::Size&,
                                        nucleus::neutron::Range<
                                          nucleus::neutron::Record>&);
      static elle::Status       Revoke(gear::Object&,
                                       const nucleus::neutron::Subject&);

      static elle::Status       Upgrade(gear::Object&,
                                        elle::cryptography::SecretKey const&);
      static elle::Status       Downgrade(gear::Object&);

      static elle::Status       Destroy(gear::Object&);
      static elle::Status       Close(gear::Object&);

      static elle::Status       Audit(gear::Object&,
                                      const nucleus::neutron::Subject&);
      static elle::Status       Regulate(gear::Object&);
    };

  }
}

#endif
