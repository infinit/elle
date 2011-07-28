//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/FUker.hh
//
// created       julien quintard   [tue jul 26 15:28:37 2011]
// updated       julien quintard   [wed jul 27 10:42:55 2011]
//

#ifndef PIG_FUKER_HH
#define PIG_FUKER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX FUSE-Broker
  ///
  class FUker:
    public elle::Entity
  {
  public:
    //
    // enumerations
    //
    static const elle::String		TypeSequential;
    static const elle::String		TypeInterlaced;
    static const elle::String		TypeParallel;

    //
    // methods
    //
    virtual elle::Status	Setup() = 0;
  };

}

#endif
