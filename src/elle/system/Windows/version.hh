#ifndef INFINIT_WINDOWS
# error this file should be included if INFINIT_WINDOWS is not defined.
#endif

namespace elle
{
  namespace system
  {
    namespace windows
    {
      // Return a formated string of the version.
      // e.g.: 7 (Professional Edition) (build 9200) (x64)
      std::string
      version_name();
    }
  }
}
