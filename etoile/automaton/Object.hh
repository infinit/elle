#ifndef ETOILE_AUTOMATON_OBJECT_HH
# define ETOILE_AUTOMATON_OBJECT_HH

# include <elle/types.hh>
# include <etoile/miscellaneous/fwd.hh>
# include <etoile/gear/fwd.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides object-related automata.
    ///
    /// these automata therefore operate on the general abstract such
    /// as the access block referenced by any object, the meta data and so
    /// on.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static elle::Status       Load(gear::Object&);

      static elle::Status       Information(gear::Object&,
                                            miscellaneous::Abstract&);

      static elle::Status       Discard(gear::Object&);
      static elle::Status       Destroy(gear::Object&);
      static elle::Status       Store(gear::Object&);
    };

  }
}

#endif
