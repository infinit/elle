#ifndef ETOILE_PORTAL_WRAPPER_HH
# define ETOILE_PORTAL_WRAPPER_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/path/fwd.hh>

# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace portal
  {

    ///
    /// this class contains wrappers for the purpose of translating pointers
    /// to internal objects into valid ready-to-be-serialized items.
    ///
    /// for more information on the actual implementation, please refer
    /// to the wall namespace.
    ///
    /// note that the original implementation has been kept in order
    /// to avoid relying on copies as it would impact the performance.
    ///
    class Wrapper
    {
    public:
      //
      // static methods
      //
      struct Access
      {
        static elle::Status     Lookup(const gear::Identifier&,
                                       const nucleus::neutron::Subject&,
                                       nucleus::neutron::Record&);
        static elle::Status     Consult(const gear::Identifier&,
                                        const nucleus::neutron::Index&,
                                        const nucleus::neutron::Size&,
                                        nucleus::neutron::Range<
                                          nucleus::neutron::Record>&);
      };

      struct Attributes
      {
        static elle::Status     Get(const gear::Identifier&,
                                    const elle::String&,
                                    nucleus::neutron::Trait&);
        static elle::Status     Fetch(const gear::Identifier&,
                                      nucleus::neutron::Range<
                                        nucleus::neutron::Trait>&);
      };

      struct Directory
      {
        static elle::Status     Lookup(const gear::Identifier&,
                                       const path::Slab&,
                                       nucleus::neutron::Entry&);
        static elle::Status     Consult(const gear::Identifier&,
                                        const nucleus::neutron::Index&,
                                        const nucleus::neutron::Size&,
                                        nucleus::neutron::Range<
                                          nucleus::neutron::Entry>&);
      };

      struct Group
      {
        /// XXX
        static
        elle::Status
        Lookup(gear::Identifier const& identifier,
               nucleus::neutron::Subject const& subject,
               nucleus::neutron::Fellow& fellow);
        /// XXX
        static
        elle::Status
        Consult(gear::Identifier const& identifier,
                nucleus::neutron::Index const& index,
                nucleus::neutron::Size const& size,
                nucleus::neutron::Range<
                  nucleus::neutron::Fellow>& range);
      };
    };

  }
}

#endif
