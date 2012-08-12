#ifndef ETOILE_WALL_ATTRIBUTES_HH
# define ETOILE_WALL_ATTRIBUTES_HH

# include <elle/types.hh>

# include <nucleus/neutron/fwd.hh>

# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace wall
  {

    ///
    /// this class provides functionalities for controlling the attributes
    /// attached to objects.
    ///
    class Attributes
    {
    public:
      //
      // static methods
      //
      static elle::Status       Set(const gear::Identifier&,
                                    const elle::String&,
                                    const elle::String&);
      static elle::Status       Get(const gear::Identifier&,
                                    const elle::String&,
                                    nucleus::neutron::Trait const*&);
      static elle::Status       Fetch(const gear::Identifier&,
                                      nucleus::neutron::Range<
                                        nucleus::neutron::Trait>&);
      static elle::Status       Omit(const gear::Identifier&,
                                     const elle::String&);
    };

  }
}

#endif
