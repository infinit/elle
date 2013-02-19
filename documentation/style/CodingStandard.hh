/*
 * The three files CodingStandard.{cc, hh, hxx} along with fwd.hh and
 * SomeInterface.hh describe the coding style used throughout the Infinit
 * project which every developer must comply to.
 *
 * Among the core rules are:
 *
 *   o Never use tabulations, always prefer spaces for both indentation
 *     and alignment.
 *   o Never exceed 80 columns: lines must be broken if necessary.
 *   o Everything must be written in English, properly using cases.
 *   o Space out the code as much as possible so as to make it breathe.
 *   o Use // for in-code comments and /// for the generated documentation
 *     i.e Doxygen. As such the class description, method descriptions,
 *     global variable descriptions etc. are considered documentation.
 *   o XXX must be included in comments in order to specify something which
 *     needs completing or re-working. Note that should the modification be
 *     important, one would have to open a ticket instead.
 *
 * All comments used for explaining the coding standards are written in
 * C-style -- i.e including this comment -- so as to be distinguishable
 * from the rest.
 *
 * The overall naming convention can be summarized as:
 *   o Types: upper CamelCase
 *   o Values (attributes, variables, enums etc.): lowercase
 */

/*
 * A single space must follow macros such as ifndef, define etc., no matter
 * their position.
 */
#ifndef PACKAGE_MODULE_CODINGSTANDARD_HH
# define PACKAGE_MODULE_CODINGSTANDARD_HH
/*
 * Note that the preprocessor definitions lying within a macro scope must
 * be indented following a one-space rule, the macro above being an example
 * of this rule along with the includes below.
 */

/*
 * First must be included the Infinit dependencies, grouped by package
 * or module. Note the brackets are used for the includes.
 *
 * Besides, the forward declarations should always be preferred, unless
 * the actual type definition is absolutely necessary.
 */
# include <package/module/fwd.hh>
# include <package/module/InfinitDepdency1.hh>
# include <package/module/InfinitDepdency2.hh>

/*
 * Then must be included the system dependencies.
 */
# include <system-dependecy.h>
# include <library-dependency.h>

/*
 * Macros and macro-functions must be named in capital with the
 * underscore acting as a seperator.
 *
 * Besides, every macro and macro-function should be prefixed by
 * the name of its module (and possibly even its package) in order
 * to prevent conflicts.
 *
 * Additionally, as one can notice, the macros and macro-functions do
 * not need to be placed in the namespaces since independent from
 * the language used.
 *
 * Finally, macro-function parameters should be prefixed and suffixed
 * by an underscore in order to prevent conflicts with variables while
 * long macro-functions should be split into several lines, indented
 * following a two-space rule and terminated by an aligned backslash,
 * as shown below.
 */
/*-------.
| Macros |
`-------*/
# define PACKAGE_MODULE_STRINGIFY(_variable_)                           \
  #_variable_

/*
 * The namespaces must be defined in lower-case with the opening and
 * closing braces on their own line and aligned with the namespace
 * keyword.
 */
namespace package
{
  namespace module
  {
/*
 * This long separator -- as opposed to the short separators which will be
 * introduced later on -- can be relied upon should several classes be defined
 * in the same file.
 *
 * This makes it clearer where the code belongs. Note however that the file
 * must be named after the main class being defined in the file, following
 * the naming convention of the class i.e capitalized CamelCase a.k.a
 * upper CamelCase.
 *
 * Noteworthy is that acroynyms such as JSON must be treated as such e.g.
 * JSONParser.
 */
//
// ---------- CodingStandard --------------------------------------------------
//

    /// The general documentation about the class purpose should be provided
    /// here.
    ///
    /// Note that every class must be named following the upper CamelCase
    /// naming convention i.e starting with an uppercase letter.
    class CodingStandard:
      /*
       * Every class deriving others must end with a colon while the derived
       * classes must be presented on their own line, indented with two spaces,
       * and ending with a comma, as shown here.
       */
      public BaseClass,
      public AnotherBaseClass,

      /*
       * Every class should privately inherit the boost::noncopyable class
       * in order to render the current class non-copyable. Should the class
       * really need to be copied, one should wonder if the copy constructor
       * should be activated, the = operator or both.
       *
       * Note that offering both the copy constructor and the = operator often
       * leads to redundant code sections since the = operator can be considered
       * as a merge between the destructor and the copy constructor. Thus, the
       * copy constructor should be used as much as possible while the = operator
       * should be provided for very important classes whose manipulation needs
       * to be eased.
       */
      private boost::noncopyable
    {
      /*
       * Below are defined the types, constants, constructor/destructor,
       * methods, accessors and attributes, each within a specific section.
       *
       * Every section must represent a logical grouping, being easily
       * identified through a short separator.
       *
       * Besides, each section should provide the openness of its members
       * being public, protected or private.
       *
       * In addition, it is always preferred to align the elements
       * grouped together in order to ease readability.
       */

      /*
       * For example, the inner tyes can be defined, using a upper
       * CamelCase notation, as shown below.
       *
       * Note that types, constants and static attributes all rely on
       * the CamelCase notation.
       */
      /*------.
      | Types |
      `------*/
    public:
      typedef Natural32 Size;

      /*
       * Enumerations should be defined as 'enum class' in order to
       * benefit from scoping.
       *
       * The values should be named in lowercase since representing values
       * but the enumeration in itself should be named in upper CamelCase
       * since representing a type.
       */
      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      /*
       * Note that opening and closing braces are always aligned on the previous
       * keyword. Thus, the keywords if, for, while, enum etc. are followed by
       * braces (on their own line) which are perfectly aligned. Then, the
       * scoped content is aligned with two spaces.
       */
      enum class State
      {
        inprogress,
        done
      };

      /*
       * Note that constants, since representing values, are named in
       * lowercase.
       */
      /*----------.
      | Constants |
      `----------*/
    public:
      static const Natural32 zero;

      /*------------------.
      | Static Attributes |
      `------------------*/
    public:
      /*
       * Comments on methods and attributes tend to be straightforward,
       * often starting with a verb rather than 'This method does that'.
       */
      /// Explain what is this static attribute.
      static Natural32 average_size;

      /*
       * Methods, unlike attributes, rely on another notation which
       * consists in lowercase words separated by underscores, as
       * show below.
       *
       * However, one should prefer naming methods using a single word
       * as it should be meaningful enough for the reader to understand
       * the action in the context of the class.
       *
       * For example, the static method generate() obviously generates
       * a CodingStyle instance.
       */
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Note that these kind of one-line (if possible) descriptions
      /// are supposed to be extracted by Doxygen since using '///'.
      ///
      /// One must note that the prototype below is a bit specific as
      /// spanning three lines. Indeed, the method qualifiers (static, inline,
      /// virtual, etc.) must be placed on their own line. The same is true
      /// for the return type and the method name.
      static
      CodingStandard*
      generate();

      /*-------------.
      | Construction |
      `-------------*/
    public:
      CodingStandard();
      /*
       * Note that explicit constructors must be used as much as possible,
       * especially for the one-argument constructors in order to prevent
       * unfortunate behaviours.
       */
      explicit
      CodingStandard(elle::Natural32 size);

      /*
       * The methods, as any other method, should be named in lowercase
       * with underscores as separators. Should it be necessary, accessors
       * should also be named this way. Note that short method names are
       * always preferred since self-explanatory given the class context.
       *
       * The name of the arguments should also be specified so as to
       * provide all the information required directly within the header file.
       *
       * Noteworthy is that every return type is placed on its own line
       * while every argument also lies on its own line. This style increases
       * readability while coping with very long types, especially template
       * types.
       */
      /*--------.
      | Methods |
      `--------*/
    public:
      /// Another description for a method which acts as an accessor i.e
      /// getter but the developer is not supposed to know that and thus
      /// cannot distinguish methods from accessors.
      Real
      something() const;

      /// Compute something based on something and somethingelse.
      Natural32
      compute(another::place::SomeOtherClass const& something,
              Natural32 const somethingelse);

      /*
       * Note that the 'const' keywords must be well-placed, i.e after
       * the type.
       *
       * Besides, even for arguments passed by copy, the constness should
       * be specified.
       */
      /// Another description for a specific method.
      template <typename T>
      void
      generate_something_specific(Natural32 const ohoh,
                                  T& bla);

    private:
      /*
       * Note that private methods, as private attributes, must be prefixed
       * by an underscore, as shown below.
       */
      Real
      _inner_action(String const& left,
                    Natural32 const right);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      /*
       * One must use the basic types provided such as Boolean,
       * Integer, Natural, Real, String, Character, Byte etc.
       *
       * Besides, attributes must be kept private and named with a
       * underscore as prefix in order to prevent conflicts with
       * method arguments i.e compute(something, somethingelse) can
       * very easily distinguish 'something' from the private attribute
       * '_something'.
       *
       * However, macro functions are provided so as to generate
       * both the private attribute along with some accessors, should
       * they be required.
       */
      /// A description for the use of this attribute.
      ELLE_ATTRIBUTE(Real, something);
      /*
       * Note however that this attribute may not need to be described
       * since its name may be self explanatory.
       *
       * This attribute has both a read and write accessor.
       */
      ELLE_ATTRIBUTE_RW(Natural32, size);
      /*
       * Pointer-based attributes within a class should make use of the
       * unique_ptr construct in order to ensure safety.
       *
       * Indeed, should an error occur during the initialization of the
       * object, the destructor is not called. Should one allocate
       * something in _implementation, it would never be released. However,
       * the compiler guarantees that every constructed attribute is destructed
       * should an error occur in the initialization. Therefore, by relying
       * on a unique_ptr, the allocated memory would be released.
       *
       * Now, given the following method in the current class:
       *
       *   void
       *   setter(Implementation const& implementation)
       *   {
       *     delete this->_implementation;
       *     this->_implementation = new Implementation{implementation};
       *   }
       *
       * Using this method would be unsafe, as shown below:
       *
       *   CodingStandard cs;
       *   Implementation implementation;
       *
       *   cs.setter(implementation);
       *
       * The method is invalid because, should the new Implementation throw,
       * the CodingStandard instance _cs_ would be destructured. However, the
       * _implementation pointer has already been deleted but already holds
       * the pointer to the old memory. Therefore, in the destructor, it will
       * try to delete the implementation once again, leading to an error.
       *
       * The right way would be to reset _implementation to nullptr after its
       * manual deletion or even better, to rely on a unique_ptr.
       */
      ELLE_ATTRIBUTE(std::unique_ptr<Implementation>, implementation);
    };
  }
}

/*
 * Every header file must automatically include its template-specific
 * header file.
 *
 * Note that the .hxx is included within the guarded section i.e
 * #ifndef ... #endif.
 */
# include <package/module/CodingStandard.hxx>

/*
 * Note that one does not need to put an indicator of the opening macro:
 *
 *   #endif // PACKAGE_MODULE_CODINGSTANDARD_HH
 *
 * Unless of course the number of nested macro scopes is important, in which
 * case it is tolerated.
 */
#endif
