#ifndef ETOILE_AUTOMATON_CONTENTS_HH
# define ETOILE_AUTOMATON_CONTENTS_HH

#include <elle/types.hh>

namespace etoile
{
  namespace automaton
  {

    ///
    /// this class provides a small set of automata for opening, closing
    /// and destorying an object's contents.
    ///
    class Contents
    {
    public:
      //
      // static methods
      //
      template <typename T>
      static elle::Status       Open(T&);
      template <typename T>
      static elle::Status       Close(T&);

      template <typename T>
      static elle::Status       Destroy(T&);
    };

  }
}

#include <etoile/automaton/Contents.hxx>

#endif
