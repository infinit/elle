#ifndef ETOILE_WALL_GROUP_HH
# define ETOILE_WALL_GROUP_HH

# include <elle/types.hh>

# include <nucleus/neutron/Fellow.hh>
# include <nucleus/neutron/Group.hh>
# include <nucleus/neutron/Range.hh>
# include <nucleus/neutron/fwd.hh>

# include <etoile/abstract/fwd.hh>
# include <etoile/gear/Identifier.hh>
# include <etoile/gear/fwd.hh>

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
      std::pair<nucleus::neutron::Group::Identity, gear::Identifier>
      Create(elle::String const& description);
      /// XXX
      static
      gear::Identifier
      Load(typename nucleus::neutron::Group::Identity const& identity);
      /// XXX
      static
      abstract::Group
      Information(const gear::Identifier& identifier);
      /// XXX
      static
      void
      Add(gear::Identifier const& identifier,
          nucleus::neutron::Subject const& subject);
      /// XXX
      static
      std::unique_ptr<const nucleus::neutron::Fellow>
      Lookup(gear::Identifier const& identifier,
             nucleus::neutron::Subject const& subject);
      /// XXX
      static
      nucleus::neutron::Range<nucleus::neutron::Fellow>
      Consult(gear::Identifier const& identifer,
              nucleus::neutron::Index const& index,
              nucleus::neutron::Size const& size);
      /// XXX
      static
      void
      Remove(gear::Identifier const& identifier,
             nucleus::neutron::Subject const& subject);
      /// XXX
      static
      void
      Discard(gear::Identifier const& identifier);
      /// XXX
      static
      void
      Store(gear::Identifier const& identifier);
      /// XXX
      static
      void
      Destroy(gear::Identifier const& identifier);
      /// XXX
      static
      elle::Status
      Purge(gear::Identifier const& identifier);
    };

  }
}

#endif
