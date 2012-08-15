#ifndef ETOILE_AUTOMATON_GROUP_HH
# define ETOILE_AUTOMATON_GROUP_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <etoile/gear/fwd.hh>
# include <etoile/abstract/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    /// This class provides methods for manipulating group contexts.
    class Group
    {
    public:
      //
      // static methods
      //
      /// XXX
      static
      elle::Status
      Create(gear::Group& context,
             elle::String const& description,
             typename nucleus::neutron::Group::Identity& identity);
      /// XXX
      static
      elle::Status
      Load(gear::Group& context);
      /// XXX
      static
      elle::Status
      Information(gear::Group& context,
                  abstract::Group& abstract);
      /// XXX
      static
      elle::Status
      Add(gear::Group& context,
          nucleus::neutron::Subject const& subject);
      /// XXX
      static
      elle::Status
      Lookup(gear::Group& context,
             nucleus::neutron::Subject const& subject,
             nucleus::neutron::Fellow const*& fellow);
      /// XXX
      static
      elle::Status
      Consult(gear::Group& context,
              nucleus::neutron::Index const& index,
              nucleus::neutron::Size const& size,
              nucleus::neutron::Range<nucleus::neutron::Fellow>& range);
      /// XXX
      static
      elle::Status
      Remove(gear::Group& context,
             nucleus::neutron::Subject const& subject);
      /// XXX
      static
      elle::Status
      Discard(gear::Group& context);
      /// XXX
      static
      elle::Status
      Destroy(gear::Group& context);
      /// XXX
      static
      elle::Status
      Store(gear::Group& context);
    };

  }
}

#endif
