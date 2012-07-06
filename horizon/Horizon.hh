#ifndef HORIZON_HORIZON_HH
# define HORIZON_HORIZON_HH

#include <elle/types.hh>

///
/// this namespace contains several implementations related to the
/// operating system as the system call interface changes.
///
namespace horizon
{

  ///
  /// this class contains general-purpose methods for initializing and
  /// cleaning the horizon component.
  ///
  class Horizon
  {
  public:
    //
    // static methods
    //
    static elle::Status         Initialize();
    static elle::Status         Clean();
  };

}

#endif
