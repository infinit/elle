#ifndef ETOILE_AUTOMATON_AUTHOR_HH
# define ETOILE_AUTOMATON_AUTHOR_HH

# include <elle/types.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides automata related to the authorship.
    ///
    class Author
    {
    public:
      //
      // static methods
      //
      static elle::Status       Forge(gear::Object&);
    };

  }
}

#endif
