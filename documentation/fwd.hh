/*
 * In this file, every class in the module gets forward declared so as
 * to speed up the compilation process.
 * pointer or reference must be forward declared in order to speed
 * up the compilation process.
 */

#ifndef PACKAGE_MODULE_FWD_HH
# define PACKAGE_MODULE_FWD_HH

/*
 * first files defining type aliases (i.e typedefs) are included.
 */
# include <package/module/SimpleType.hh>

namespace package
{
  namespace module
  {

    class CodingStandard;
    class SomeOtherClass;

  }
}

#endif
