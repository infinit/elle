//
// ---------- header ----------------------------------------------------------
//
// project      documentation
//
// license      infinit
//
// author       Raphael Londeix   [Thu 26 Jan 2012 06:32:31 PM CET]
//

#ifndef PACKAGE_MODULE_CODINGSTANDARD_HH
#define PACKAGE_MODULE_CODINGSTANDARD_HH

//
// ---------- information -----------------------------------------------------
//
// the three files CodingStandard.{cc, hh, hxx} describes the coding style
// used throughout the Infinit project which every developer must comply to.
//
// among the core rules are:
//
//   o never use tabulations, always prefer spaces for both indentation
//     and alignment.
//   o never exceed 80 columns: lines must be broken if necessary
//   o never use underscores for naming i.e CodingStandard will always be
//     preferred to Coding_Standard or coding_standard.
//   o everything must be written in English and lower-case as much as
//     possible.
//   o space out the code as much as possible so as to make it breathe :)
//   o use // for in-code comments and /// for the generated documentation
//     i.e Doxygen. as such the class description, method descriptions,
//     global variable descriptions etc. are considered documentation.
//   o XXX must be included in comments in order to specify something which
//     needs completing or re-working.
//
// all comments used for explaining the coding standards are written in
// C-style i.e /* comment about the coding style */.
//

//
// ---------- includes --------------------------------------------------------
//

/*
 * first must be included the Infinit dependencies, grouped by package
 * or module.
 *
 * note the brackets are used for the includes.
 */
#include <infinit-dependency-1.hh>
#include <infinit-dependency-2.hh>

/*
 * then must be included the system dependencies.
 *
 * note that in order to prevent conflicts, these are enclosed between
 * an idiom Close and Open in order to undefine the macros and redefine
 * them. however, these macros will soon be removed so this should no
 * longer be necessary.
 */
#include <idiom/Close.hh>
# include <system-dependecy.h>
# include <library-dependency.h>
#include <idiom/Open.hh>

/*
 * the namespaces must be defined in lower-case with the opening and
 * closing brackets on their own line.
 */
namespace package
{
  namespace module
  {

//
// ---------- forward declarations --------------------------------------------
//

    /*
     * every class which is referenced in the header file as a
     * pointer or reference must be forward declared in order to speed
     * up the compilation process.
     */
    class SomeOtherClass;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the general documentation about the class purpose.
    ///
    class CodingStandard:
      /*
       * note that every class being derived is specified on its own line.
       */
      public DerivedClass,
      public AnotherDerivedClass
    {
      //
      // types
      //
    public:
      typedef Natural32         Type1;

      //
      // constants
      //
    public:
      const static Natural32    Constant1;

      /*
       * note that structures are always preferred to extended names
       * such as DefaultConstant2.
       *
       * using structures, including for attributes, making is more
       * natural since splitting the containing entity 'Default' from the
       * actual name 'Constant2'.
       */
      struct                    Default
      {
        const static Real       Constant2;
      };

      //
      // static attributes
      //
    public:
      static Natural32          Attribute1;

      //
      // static methods
      //
    public:
      static Status     Method1();

      //
      // constructors & destructors
      //
    public:
      CodingStandard();
      ~CodingStandard();

      //
      // getters & setters
      //
    public:
      /*
       * the getters and setters must be used as little as
       * possible.
       *
       * however they are sometimes necessary since attributes
       * must be kept private.
       *
       * setters and getters take the name of the attribute
       * and are overloaded in order to distinguish them.
       */
      Real              attribute1() const;
      Void              attribute1(const Real&);

      //
      // methods
      //
      /*
       * note that the name of the method arguments are not provided
       * in the header as implementation specific.
       */
    public:
      Status            Method2(SomeOtherClass&,
                                const Natural32);
      template <typename T>
      Status            Method3(const Natural32,
                                T&);
    private:
      Status            Method4(const String&,
                                const Natural32,
                                Real&);

      //
      // attributes
      //
    private:
      /*
       * one must use the basic types provided by Elle: Void, Boolean,
       * Integer, Natural, Real, String, Character, Byte etc.
       *
       * besides, attributes must be kept private (though most of the
       * code must still be adapted to this rule) and named with a
       * underscore as prefix in order to prevent conflicts
       * with method arguments.
       */
      Real              _attribute1;
      Natural32         _attribute2;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <package/module/CodingStandard.hxx>

#endif
