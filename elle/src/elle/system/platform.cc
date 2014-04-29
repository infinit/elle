#include <elle/system/platform.hh>

namespace elle
{
  namespace system
  {
    namespace platform
    {
      std::string
      name()
      {
#if defined INFINIT_WINDOWS
        return "Windows";
#elif defined INFINIT_LINUX
        return "Linux";
#elif defined INFINIT_MACOSX
        return "MacOSX";
#else
# error Please define INFINIT_{OS} according to your platform.
#endif
      }
    }
  }
}
