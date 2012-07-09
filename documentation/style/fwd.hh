/*
 * In this file, every class in the module gets forward declared so as
 * to speed up the compilation process.
 * Pointers and references must be forward declared in order to speed
 * up the compilation process.
 *
 * Note that all files which do not describe a class must be named in
 * lowercase: fwd.hh, all.hh, log.hh etc.
 */

#ifndef PACKAGE_MODULE_FWD_HH
# define PACKAGE_MODULE_FWD_HH

/*
 * First files defining type aliases (i.e typedefs) are included.
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
