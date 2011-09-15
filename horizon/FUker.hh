//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien quintard   [tue jul 26 15:28:37 2011]
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
  /// this class abstract the notion of FUSE-Broker i.e FUker.
  ///
  /// FUkers have been introduced in order to merge the FUSE-specific
  /// event loop with Elle's. this way all the events are processed by
  /// a single event loop, Elle's.
  ///
  /// this is accomplished by implementing a broker for the FUSE events.
  /// whenever data is available on the FUSE special device, the broker
  /// triggers an event which makes the FUker read the data and handle the
  /// event. the rest of the time, nothing FUSE-related happens so that
  /// other Elle-specific events can be treated.
  ///
  class FUker:
    public elle::Entity
  {
  public:
    //
    // enumerations
    //
    enum
      {
	TypeSequential,
	TypeInterlaced,
	TypeParallel
      };

    //
    // methods
    //
    virtual elle::Status	Setup() = 0;
  };

}

#endif
