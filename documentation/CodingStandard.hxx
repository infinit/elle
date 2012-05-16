#ifndef PACKAGE_MODULE_CODINGSTANDARD_HXX
# define PACKAGE_MODULE_CODINGSTANDARD_HXX

# include <system-dependecy.h>
# include <library-dependency.h>

/*
 * The namespaces are re-specified.
 */
namespace package
{
  namespace module
  {

    /*--------.
    | Methods |
    `--------*/

    /*
     * So as to make things clear, every C++ keyword must be followed
     * by a single space. These keywords include, among others: return,
     * sizeof, if, for, switch and template.
     *
     * Once again, one can notice that the return type is specified on
     * its own line while arguments also lie on their line.
     */
    template <typename T>
    void
    generate_something_specific(Natural32 const ohoh,
                                T& bla)
    {
      //
      // One can comment some parts of the code. However, these comments
      // do not need to be exposed to the developer using the class.
      //
      // Therefore, one can notice that this comment is not intended to
      // be extracted by Doxygen.
      //
    }

  }
}

#endif
