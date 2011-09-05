//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Author.hh
//
// created       julien quintard   [mon jun 20 14:12:54 2011]
// updated       julien quintard   [sun sep  4 13:49:11 2011]
//
 
#ifndef ETOILE_AUTOMATON_AUTHOR_HH
#define ETOILE_AUTOMATON_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Object.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides automata related to the authorship.
    ///
    class Author
    {
    public:
      //
      // static methods
      //
      static elle::Status	Forge(gear::Object&);
    };

  }
}

#endif
