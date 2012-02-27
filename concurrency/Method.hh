//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu feb  4 23:03:30 2010]
//

#ifndef ELLE_CONCURRENCY_METHOD_HH
#define ELLE_CONCURRENCY_METHOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// Method generic class.
    ///
    template <typename... T>
    class Method;

    ///
    /// this class represents a method.
    ///
    template <typename R,
              typename... T>
    class Method< R,
                  Parameters<T...> >:
      public Object
    {
    public:
      //
      // types
      //
      typedef Parameters<T...>          P;

      //
      // classes
      //

      ///
      /// the Shell and Wrap classes are required so that the Method
      /// class can be retrieved from T..., meaning without C which is
      /// only used at the Method construction time.
      ///
      /// this class is a base for inheritance.
      ///
      class Shell:
        public Object
      {
      public:
        //
        // methods
        //
        virtual R       Call(T...) = 0;
        virtual Void    Trigger(T...) = 0;
      };

      ///
      /// this class is C-specific.
      ///
      template <typename C>
      class Wrap:
        public Shell
      {
      public:
        //
        // types
        //
        typedef R                       (C::*Handler)(T...);

        //
        // constructors & destructors
        //
        Wrap(Handler,
             C*);

        //
        // methods
        //
        R               Call(T...);
        Void            Trigger(T...);

        //
        // interfaces
        //

        // object
        declare(Wrap<C>);

        // dumpable
        Status          Dump(const Natural32 = 0) const;

        //
        // attributes
        //
        C*              object;
        Handler         handler;
      };

      //
      // constructors & destructors.
      //
      template <typename C>
      Method(R (C::*)(T...),
             C*);
      Method(const Method<R, P>&);
      ~Method();

      //
      // methods
      //
      R                 Call(T...);
      Void              Trigger(T...);

      //
      // interfaces
      //

      // object
      declare(_(Method<R, P>));

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Shell*            shell;
    };

    ///
    /// a specific class for method inference.
    ///
    template <>
    class Method<>
    {
    public:
      //
      // static methods
      //
      template <typename R,
                typename C,
                typename... T>
      static Method< R, Parameters<T...> >      Infer(R (C::*)(T...),
                                                      C*);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Method.hxx>

#endif
