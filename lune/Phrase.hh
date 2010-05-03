//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/lune/Phrase.hh
//
// created       julien quintard   [sat may  1 12:52:01 2010]
// updated       julien quintard   [sat may  1 12:53:53 2010]
//

#ifndef ELLE_LUNE_PHRASE_HH
#define ELLE_LUNE_PHRASE_HH

namespace elle
{
  namespace lune
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a phrase i.e a string identifying a user
    /// by linking applications with the user's agent.
    ///
    class Phrase:
      public Entity,
      public Dumpable, public Fileable<FormatRaw>
    {
    };

  }
}

#endif
