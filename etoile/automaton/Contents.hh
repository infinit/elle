//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 16:26:27 2011]
//

#ifndef ETOILE_AUTOMATON_CONTENTS_HH
#define ETOILE_AUTOMATON_CONTENTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

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
      static elle::Status	Open(T&);
      template <typename T>
      static elle::Status	Close(T&);

      template <typename T>
      static elle::Status	Destroy(T&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/automaton/Contents.hxx>

#endif
