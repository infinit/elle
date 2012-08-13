#ifndef ETOILE_AUTOMATON_RIGHTS_HH
# define ETOILE_AUTOMATON_RIGHTS_HH

# include <elle/types.hh>
# include <etoile/gear/fwd.hh>
# include <nucleus/neutron/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides automata for computing the user's rights
    /// over a given object context.
    ///
    class Rights
    {
    public:
      //
      // static methods
      //
      static elle::Status       Determine(gear::Object&);
      static elle::Status       Update(gear::Object&,
                                       const nucleus::neutron::Permissions&);
      /// XXX
      static
      elle::Status
      Determine(gear::Group&);
      /// XXX
      template <typename T>
      static
      elle::Status
      Recompute(T&);
      /// XXX
      template <typename T>
      static
      elle::Status
      Operate(T&,
              const gear::Operation&);
    };

  }
}

# include <etoile/automaton/Rights.hxx>

#endif
