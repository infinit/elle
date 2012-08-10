#ifndef ETOILE_AUTOMATON_ENSEMBLE_HH
# define ETOILE_AUTOMATON_ENSEMBLE_HH

# include <elle/types.hh>

# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    /// XXX
    class Ensemble
    {
    public:
      //
      // static methods
      //
      /// XXX
      static
      elle::Status
      Open(gear::Group&);
      /// XXX
      static
      elle::Status
      Close(gear::Group&);
      /// XXX
      static
      elle::Status
      Destroy(gear::Group&);
    };

  }
}

#endif
