#ifndef HORIZON_HORIZON_HH
# define HORIZON_HORIZON_HH

# include <elle/types.hh>

# include <lune/Dictionary.hh>

# include <elle/idiom/Close.hh>
#  include <sys/types.h>
#  include <pwd.h>
# include <elle/idiom/Open.hh>

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

    //
    // static attributes
    //
    struct                    Somebody
    {
      static uid_t            UID;
      static gid_t            GID;
    };

    static lune::Dictionary   Dictionary;
  };

}

#endif
