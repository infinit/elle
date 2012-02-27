//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [thu jul 28 17:01:13 2011]
//

#ifndef LUNE_LOG_HH
#define LUNE_LOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the local log file.
  ///
  class Log:
    public elle::Object
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;
  };

}

#endif
