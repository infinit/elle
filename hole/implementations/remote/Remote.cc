#include <hole/implementations/remote/Remote.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      /// This global variable makes the machine i.e the current host
      /// accessible from every instance of the implementation.
      Machine*                          Remote::Computer = nullptr;
    }
  }
}
