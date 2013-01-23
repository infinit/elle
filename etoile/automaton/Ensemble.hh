#ifndef ETOILE_AUTOMATON_ENSEMBLE_HH
# define ETOILE_AUTOMATON_ENSEMBLE_HH

# include <elle/types.hh>

# include <etoile/gear/fwd.hh>

// XXX
#define ENSEMBLE_SECRET_KEY_LENGTH 13
#define ENSEMBLE_SECRET_KEY "no-secret-key"

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
