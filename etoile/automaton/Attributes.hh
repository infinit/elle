#ifndef ETOILE_AUTOMATON_ATTRIBUTES_HH
# define ETOILE_AUTOMATON_ATTRIBUTES_HH

# include <elle/types.hh>
# include <nucleus/neutron/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides functionalities for manipulating an
    /// object's attributes.
    ///
    class Attributes
    {
    public:
      //
      // static methods
      //
      static
      void
      open(gear::Object& context);

      static elle::Status       Set(gear::Object&,
                                    const elle::String&,
                                    const elle::String&);
      static elle::Status       Get(gear::Object&,
                                    const elle::String&,
                                    nucleus::neutron::Trait const*&);
      static elle::Status       Fetch(gear::Object&,
                                      nucleus::neutron::Range<
                                        nucleus::neutron::Trait>&);
      static elle::Status       Omit(gear::Object&,
                                     const elle::String&);

      static
      void
      destroy(gear::Object& context);
      static
      void
      close(gear::Object& context);
    };

  }
}

#endif
