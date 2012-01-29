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
// ---------- includes --------------------------------------------------------
//

#include <infinit-dependency-1.hh>
#include <infinit-dependency-2.hh>

#include <idiom/Close.hh>
# include <system-dependecy.h>
# include <library-dependency.h>
#include <idiom/Open.hh>

namespace package
{
  namespace module
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// general documentation about the class.
    ///
    //   o never use tabulations
    //   o never exceed 80 columns
    //   o never use underscores, prefer SuchMethod rather than such_method
    ///
    class CodingStandard:
      public DerivedClass,
      public AnotherDerivedClass
    {
      //
      // types
      //
    public:
      typedef Natural32         Type1;
    protected:
      typedef Natural32         Type2;
    private:
      typedef Natural32         Type3;

      //
      // constants
      //
    public:
      static const Natural32    Constant1;

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

      //
      // methods
      //
    public:
      Status            Method2();
      template <typename T>
      Status            Method3(const Natural32,
                                T&);
    private:
      Status            Method4(const String&);

      //
      // attributes
      //
    public:
      Natural8          attribute1;
    protected:
      String            attribute2;
    private:
      Real              attribute3;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <package/module/CodingStandard.hxx>

#endif
