/*
 * It is very important to maintain clarity in source files. For that, empty
 * line should never occur at the beginning or at the end of a file.
 *
 * In other words, empty lines serve the purpose of separating content and
 * a single empty line should be used for that purpose, never more.
 */

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
    /*
     * As a general rule, parentheses, construction braces, commas, semicolons
     * etc. should never be alone on their line. The following constructs are
     * therefore *invalid*:
     *
     *   static Integer8 const reverse[256] = {
     *     -1,-1,-1,-1,-1,-1
     *   };
     *
     * The closing brace should have been put following the last -1, not to
     * mention that every comma should be followed by a space.
     *
     *   template <
     *     ArchiveMode mode
     *     , typename CharType
     *     > struct DefaultStreamTypeSelect;
     *
     * The comma should have been put after _mode_.
     *
     *   val =
     *     (static_cast<int16_t>(tab[0])      ) +
     *     (static_cast<int16_t>(tab[1]) << 8 )
     *     ;
     *
     * In this case, the spaces are unecessary and the semicolon should have
     * been put after the last closing parenthesis.
     *
     *   alive_pointer<typename std::remove_reference<T>::type>(
     *     std::forward<T>(ptr)
     *     );
     *
     * In this case, once again, the closing parenthesis should be put on the
     * last line, rather than being by itself i.e 'std::forwawrd<T>(ptr));'.
     *
     *   AlivePointer<T> alive_pointer = new AlivePointer
     *     (nullptr);
     *
     * Likewise, this is invalid open parentheses should never start a line.
     */

    /*------------.
    | Definitions |
    `------------*/

    Natural32 CodingStandard::average_size = 42;

    /*---------------.
    | Static Methods |
    `---------------*/

    CodingStandard*
    CodingStandard::generate()
    {
      /*
       * Pointers must always be initialized with 'nullptr'. Note that
       * the old C macro NULL must never be used.
       *
       * Besides, note how the star is aligned with the type rather than
       * the variable name. This is how one must do it.
       */
      CodingStandard* coding_standard = nullptr;

      /*
       * Note that braces {} should be preferred for object construction because
       * the standard is ambiguous in some cases with parentheses, as shown
       * below. The following example does not compile because the compiler
       * thinks a inner function named 'plain' is defined, returning a Plain
       * and taking an argument whose name is named 'buffer' and of type
       * WeakBuffer.
       *
       *   Plain plain(WeakBuffer(buffer));
       *
       * Using braces makes it easy for the compiler to distinguish object
       * construction (classes and structures) from function calls.
       */
      coding_standard = new CodingStandard{1, 5};

      /*
       * Every block of logically related operations should be commented so
       * as to explain what is being done.
       */
      // Comment describing the following operations.

      /*
       * Note that the identation is always composed of two characters
       * for the scoped content, not the braces:
       *
       *   for (...)
       *   {
       *   >>int i;
       *   }
       */
      for (; iterator != end; ++iterator)
      {
        // Something important is done here.
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
      /*
       * Noteworthy is that writing {} for empty constructors is tolerated.
       */
    }

    /*--------.
    | Methods |
    `--------*/

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
    _inner_action(String const& left,
                  Natural32 const right)
    {
      /*
       * One must declare one local variable per line and *never* use
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

      /*
       * For switch and cases, one should prefer the use of braces when the
       * case's content is more than a line long. A single line could suffice
       * otherwise.
       *
       * Note that indentation which is, as usual, two spaces for the content,
       * never for the braces.
       */
      switch (right)
      {
        case 0:
          break;
        default:
        {
          // Something to do here.
        }
      }

      /*
       * It is always preferable for booleans to be compared fully
       * rather than to rely on the unary operator. For example,
       * the following could have been written:
       *
       *   if (!this->_implementation)
       *   {
       *   }
       *
       * which is less expressive than the following.
       */
      if (this->_implementation == false)
      {
      }

      return (3.14);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    CodingStandard::print(std::ostream& stream) const
    {
      /*
       * Operators should never be put at the beginning of a new line i.e
       * the following is invalid:
       *
       *   assert(mode == elle::serialize::ArchiveMode::Input
       *          ||  mode == elle::serialize::ArchiveMode::Output);
       *
       * The only exception is for stream operators << and >> which
       * can be put at the beginning of the line, as shown below.
       */

      stream << "coding"
             << " "
             << "standard";
    }

    /*
     * As for macro-based scopes, closing namespace should not specify the
     * namespace being closed, unless it is necessary.
     */
  }
}
