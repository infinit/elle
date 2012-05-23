#include <package/module/CodingStandard.hh>

#include <system-dependecy.h>
#include <library-dependency.h>

/*
 * The namespaces are re-specified.
 *
 * Note that the only allowed 'using namespace' are in the implementation
 * file for its own module, or in a function scope when it improves
 * readability:
 *
 * void f()
 * {
 *    namespace tcp = boost::asio::tcp;
 *    tcp::socket s(io_service);
 * }
 *
 */
namespace package
{
  namespace module
  {

    /*------------.
    | Definitions |
    `------------*/

    const Natural32 CodingStandard::zero = 0;

    Natural32 CodingStandard::average_size = 42;

    /*---------------.
    | Static Methods |
    `---------------*/

    CodingStandard*
    CodingStandard::generate()
    {
      CodingStandard* coding_standard = new CodingStandard;

      /*
       * every block of logically related operations must be commented so
       * as to explain what is being done.
       */
      //
      // comment describing the following operations.
      //

      /*
       * note that the identation is always composed of two characters
       * even for the brackets following a for (...) or if (...):
       *
       *   for (...)
       *   >>{
       *   >>>>int i;
       *   >>}
       */
      for (; iterator != end; ++iterator)
        {
          // something important is done here.
        }

      return (coding_standard);
    }

    /*-------------.
    | Construction |
    `-------------*/

    CodingStandard::CodingStandard():
      /*
       * The constructor must be directly followed by a colon, without a space
       * in between.
       *
       * Additionally, every attribute must be initialized on its own line,
       * as shown below.
       */
      _something(1.1),
      _size(2)
    {
    }

    CodingStandard::~CodingStandard()
    {
    }

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
     * Every type must be specified using its complete namespace hierarchy.
     * As a corollary, namespace should be kept short.
     */
    Natural32
    CodingStandard::compute(another::place::SomeOtherClass const& something,
                            Natural32 const somethingelse)
    {
      /*
       * Note that every call to a static method, even if local to the
       * current class, should be prefixed by the name of the class,
       * as shown below.
       */
      CodingStandard coding_standard = CodingStandard::generate();

      /*
       * Sometimes, it is more readable to put every argument
       * of a method call on its own line rather than having an
       * extremely long call.
       */
      return (something.encrypt(somethingelse,
                                static_cast<Natural64>(this->_size)));
    }

    Real
    inner_action(String const& left,
                 Natural32 const right)
    {
      /*
       * one must declare one local variable per line and never use
       * delcarations such as:
       *
       *   auto iterator = list.begin(),
       *        end = list.end();
       *
       * where it is difficult to know if two variables are being declared
       * or a single one with a complicated initialization expression.
       */
      Natural32 length;
      Natural32 i;

      return (3.14);
    }

  }
}
