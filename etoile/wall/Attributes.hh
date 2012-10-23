#ifndef ETOILE_WALL_ATTRIBUTES_HH
# define ETOILE_WALL_ATTRIBUTES_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace wall
  {

    /// This class provides functionalities for controlling the attributes
    /// attached to objects.
    class Attributes
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Set an attribute for the given object.
      static
      void
      set(gear::Identifier const& identifier,
          elle::String const& name,
          elle::String const& value);
      /// Get the value associated with the given attribute's name.
      static
      nucleus::neutron::Trait
      get(gear::Identifier const& identifier,
          elle::String const& name);
      /// Return a set of traits associated with the given object.
      static
      nucleus::neutron::Range<nucleus::neutron::Trait>
      fetch(gear::Identifier const& identifier);

      static elle::Status       Omit(const gear::Identifier&,
                                     const elle::String&);
    };

  }
}

#endif
