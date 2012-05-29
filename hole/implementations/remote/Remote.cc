
#include <elle/network/Locus.hh>
#include <elle/utility/Time.hh>

#include <hole/implementations/remote/Remote.hh>

#include <hole/Hole.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character             Component[] = "remote";

      ///
      /// this global variable makes the machine i.e the current host
      /// accessible from every instance of the implementation.
      ///
      Machine*                          Remote::Computer = NULL;

    }
  }
}
