#ifndef HORIZON_LINUX_LINUX_HH
# define HORIZON_LINUX_LINUX_HH

# include <elle/types.hh>

# include <lune/Dictionary.hh>

# include <elle/idiom/Close.hh>
#  include <sys/types.h>
#  include <pwd.h>
# include <elle/idiom/Open.hh>

namespace horizon
{
  ///
  /// this namespace contains all the elements related to the Linux horizon i.e
  /// interface between the kernel and Infinit.
  ///
  namespace linux
  {

    ///
    /// this class contains the general-purpose information related to the
    /// Linux horizon.
    ///
    class Linux
    {
    public:
      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

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
}

#endif
