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

    Real
    CodingStandard::something() const
    {
      /*
       * The return statement must be used with parentheses since it is
       * a keyword very much as if (...), sizeof (...), for (...) etc.
       *
       * Besides, one must understand two rules when accessing attributes.
       *
       * Firstly, attributes are named with a prefixed underscore in order
       * to more easily detect wrong modifications in names; for instance
       * one renaming the argument 'attribute1' in something else would
       * note see any error (assuming no underscore prefix is used for
       * attributes) because naming 'attribute1' without 'this->' would
       * still be valid. By prefixing attributes, an error would be
       * generated.
       *
       * Secondly, 'this->' must be used *at all times* in order to
       * make it clear that the attribute is accessed while limiting
       * further naming problems.
       */
      return (this->_something);
    }

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
