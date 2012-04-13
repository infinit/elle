//
// ---------- header ----------------------------------------------------------
//
// project       documentation
//
// license       infinit
//
// author        julien quintard   [fri jan 27 15:07:03 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <package/module/CodingStandard.hh>

#include <infinit-dependency-1.hh>
#include <infinit-dependency-2.hh>

#include <idiom/Close.hh>
# include <system-dependecy.h>
# include <library-dependency.h>
#include <idiom/Open.hh>

/*
 * the 'using namespace' is used to reduce the indentation
 * in source files.
 *
 * The only allowed 'using namespace' are in the implementation file for its
 * own module, or in a function scope when it improves readability.
 *
 * void f()
 * {
 *    using namespace tcp = boost::asio::tcp;
 *    tcp::socket s(io_service);
 * }
 *
 */
using namespace package::module;

//
// ---------- definitions -----------------------------------------------------
//

///
/// this variable is a constant.
///
const Natural32             CodingStandard::Constant1;

///
/// this variable is a static attribute.
///
Natural32                   CodingStandard::Attribute1;

//
// ---------- static methods --------------------------------------------------
//

///
/// this description explains what the method does.
///
Status              CodingStandard::Method1()
{
  /*
   * one must align the variable declarations so as to easily and quickly
   * identify the type and name.
   *
   * besides, note that the * character for pointer declarations is
   * kept close to the type, not the variable's name.
   */
  std::list<Natural32>*     list;

  list = new std::list<Natural32>;

  /*
   * one must rely on the C++11 features such as auto in order to recude
   * the code size though it may sometimes not be clear what type is
   * being used.
   *
   * the developer should therefore use auto when its use is unambiguous.
   */
  auto                      iterator = list->begin();
  auto                      end = list->end();

  /*
   * every block of logically related operations must be commented so
   * as to explain what is being done.
   */
  //
  // comment describing the following operations.
  //

  /*
   * note that the identation is always composed of two characters
   * even for the brackets following a for (...) or if (...)
   */
  for (; iterator != end; ++iterator)
    {
      [...]
    }

  delete list;

  return Status::Ok;
}

//
// ---------- constructors & destructors --------------------------------------
//

///
/// description of the constructor.
///
CodingStandard::CodingStandard():
  /*
   * every attribute is initialized on its own line.
   */
  _attribute1(1.1),
  _attribute2(2)
{
}

//
// ---------- accessors -------------------------------------------------------
//

/*
 * note that getters and setters do not need description unless required for
 * a specific reason.
 */

Real                CodingStandard::attribute1()
{
  /*
   * the return statement must be used with parentheses since it is
   * a keyword very much as if (...), sizeof (...), for (...) etc.
   *
   * however, this rule is relatively flexible as most
   * 'return Status::Ok' are performed without parentheses.
   */
  return (this->_attribute1);
}

/*
 * note that setters should always return Void.
 */

Void               CodingStandard::attribute1(const Real&  attribute1)
{
  /*
   * one must understand two rules when accessing attributes.
   *
   * firstly, attributes are named with a prefixed underscore in order
   * to more easily detect wrong modifications in names; for instance
   * one renaming the argument attribute1 in somethingelse1 would
   * note see any error (assuming no underscore prefix is used for
   * attributes) because naming 'attribute1' without 'this->' would
   * still be valid. by prefixing attributes, an error would be
   * generated.
   *
   * secondly, 'this->' must be used *at all times* in order to
   * make it clear that the attribute is accessed while limiting
   * further naming problems.
   */
  this->_attribute1 = attribute1;
}

//
// ---------- methods ---------------------------------------------------------
//

///
/// description of the method.
///
/*
 * one must align the method arguments so as to easily detect the
 * type and name of the arguments.
 */
Status              CodingStandard::Method(SomeOtherClass&          soc,
                                           const Natural32          index)
{
  /*
   * note that C++ casts such as static_cast, dynamic_cast and
   * reinterpret_cast must be used instead of old-style C casts.
   *
   * note that sometimes, it is more readable to put every argument
   * of a method call on its own line rather than having an
   * extremely long call.
   */
  soc.ComputeSomething(index,
                       static_cast<Natural64>(this->_attribute2));

  return Status::Ok;
}

///
/// description of the method.
///
/*
 * every method which needs to return information cannot do it through
 * the return value, with the exception of getters of course.
 *
 * therefore, an output argument is used for that purpose. note that
 * such output arguments always follow input arguments which are
 * set as 'const' since immutable, as shown in this 'Method4'.
 */
Status              CodingStandard::Method4(const String&           input1,
                                            const Natural32         input2,
                                            Real                    output3)
{
  /*
   * one must declare one local variable per line and never use
   * delcarations such as:
   *
   *   auto         iterator = list.begin(),
   *                  end = list.end();
   *
   * where it is difficult to know if two variables are being declared
   * or a single one with a complicated initialization expression.
   */
  Natural32         length;
  Natural32         i;

  //
  // do something.
  //

  if (string.empty() == true)
    /*
     * errors must be returned through the escape() macro-function
     * until another way is used in order to avoid using macro-functions:
     *
     *   return Error("this string is empty and should not");
     *
     * note that exceptions are never thrown!
     */
    escape("the string is empty and should not");

  if (input2 > 42)
    {
      outpu3 = 0.1;
    }
  else
    {
      output3 = 0.2;
    }

  return Status::Ok;
}
