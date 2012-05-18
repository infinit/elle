#ifndef PACKAGE_MODULE_SOMEINTERFACE_HH
# define PACKAGE_MODULE_SOMEINTERFACE_HH

# include <system-dependecy.h>
# include <library-dependency.h>

namespace package
{
  namespace module
  {

    /*
     * This file describes how to define a static interface.
     *
     * Unlike other languages where methods must be located in a class,
     * the Infinit coding standard specifies that such an interface should
     * be embedded within a namespace.
     */
    ///
    /// Description of the whole interface's purpose.
    ///
    namespace some_interface:
    {
      /*--------.
      | Methods |
      `--------*/
    public:
      /// Description.
      void
      method1(Natural64 const) const;

      /// Description.
      void
      method2(Natural64&);
    };

  }
}

#endif
