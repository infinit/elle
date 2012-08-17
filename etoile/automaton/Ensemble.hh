#ifndef ETOILE_AUTOMATON_ENSEMBLE_HH
# define ETOILE_AUTOMATON_ENSEMBLE_HH

# include <elle/types.hh>

# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    /// This class provides methods for manipulating ensembles
    /// related to a group context.
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
