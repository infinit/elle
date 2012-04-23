#ifndef LUNE_LOG_HH
# define LUNE_LOG_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the local log file.
  ///
  class Log:
    public elle::radix::Object
  {
  public:
    //
    // constants
    //
    static const elle::String           Extension;
  };

}

#endif
