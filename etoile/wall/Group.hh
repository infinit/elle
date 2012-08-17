#ifndef ETOILE_WALL_GROUP_HH
# define ETOILE_WALL_GROUP_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>
# include <nucleus/neutron/Group.hh>

# include <etoile/gear/fwd.hh>
# include <etoile/abstract/fwd.hh>

namespace etoile
{
  namespace wall
  {

    /// This class provides methods for manipulating groups.
    class Group
    {
    public:
      //
      // static methods
      //
      /// XXX
      static
      elle::Status
      Create(elle::String const& description,
             typename nucleus::neutron::Group::Identity& identity,
             gear::Identifier& identifier);
      /// XXX
      static
      elle::Status
      Load(typename nucleus::neutron::Group::Identity const& identity,
           gear::Identifier& identifier);
      /// XXX
      static
      elle::Status
      Information(const gear::Identifier& identifier,
                  abstract::Group& abstract);
      /// XXX
      static
      elle::Status
      Add(gear::Identifier const& identifier,
          nucleus::neutron::Subject const& subject);
      /// XXX
      static
      elle::Status
      Lookup(gear::Identifier const& identifier,
             nucleus::neutron::Subject const& subject,
             nucleus::neutron::Fellow const*& fellow);
      /// XXX
      static
      elle::Status
      Consult(gear::Identifier const& identifer,
              nucleus::neutron::Index const& index,
              nucleus::neutron::Size const& size,
              nucleus::neutron::Range<nucleus::neutron::Fellow>& range);
      /// XXX
      static
      elle::Status
      Remove(gear::Identifier const& identifier,
             nucleus::neutron::Subject const& subject);
      /// XXX
      static
      elle::Status
      Discard(gear::Identifier const& identifier);
      /// XXX
      static
      elle::Status
      Store(gear::Identifier const& identifier);
      /// XXX
      static
      elle::Status
      Destroy(gear::Identifier const& identifier);
      /// XXX
      static
      elle::Status
      Purge(gear::Identifier const& identifier);
    };

  }
}

#endif
