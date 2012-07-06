#include <hole/implementations/local/Local.hh>

#include <elle/utility/Time.hh>

#include <Infinit.hh>

namespace hole
{
  namespace implementations
  {
    namespace local
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character             Component[] = "local";

      ///
      /// this variable is set global in order for any instance of the
      /// local implementation to retrieve the machine i.e the current
      /// host.
      ///
      Machine*                          Local::Computer = nullptr;

    }
  }
}
