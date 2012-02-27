//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar 24 23:43:50 2010]
//

#ifndef ELLE_CONCURRENCY_CALLBACK_HXX
#define ELLE_CONCURRENCY_CALLBACK_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines a null callback.
    ///
    template <typename R,
              typename... T>
    const Callback< R,
                    Parameters<T...> >  Callback< R,
                                                  Parameters<T...> >::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename R,
              typename... T>
    Callback< R, Parameters<T...> >::Callback():
      Routine::Routine(Routine::SchemeUnknown)
    {
    }

    ///
    /// function-based constructor.
    ///
    template <typename R,
              typename... T>
    Callback< R, Parameters<T...> >::Callback(
                                       typename
                                         Function<
                                           R,
                                           Parameters<
                                             T...
                                             >
                                           >::Handler           handler):
      Routine::Routine(Routine::SchemeFunction),

      function(new Function< R, Parameters<T...> >(handler))
    {
    }

    ///
    /// method-based constructor.
    ///
    template <typename R,
              typename... T>
    template <typename C>
    Callback< R, Parameters<T...> >::Callback(
                                       typename
                                         Method<
                                           R,
                                           Parameters<
                                             T...
                                             >
                                           >::
                                           template
                                             Wrap<C>::Handler   handler,
                                       C*                       object):
      Routine::Routine(Routine::SchemeMethod),

      method(new Method< R, Parameters<T...> >(handler, object))
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename R,
              typename... T>
    Callback< R, Parameters<T...> >::Callback(
                                       const
                                         Callback<
                                           R,
                                           Parameters<
                                             T...
                                             >
                                           >&                   callback):
      Routine(callback)
    {
      // copy the content depending on the scheme.
      switch (this->scheme)
        {
        case Routine::SchemeFunction:
          {
            // clone the function.
            this->function =
              new Function< R, Parameters<T...> >(*callback.function);

            break;
          }
        case Routine::SchemeMethod:
          {
            // clone the method.
            this->method =
              new Method< R, Parameters<T...> >(*callback.method);

            break;
          }
        case Routine::SchemeUnknown:
          {
            break;
          }
        }
    }

    ///
    /// destructor.
    ///
    template <typename R,
              typename... T>
    Callback< R, Parameters<T...> >::~Callback()
    {
      // release the the content.
      switch (this->scheme)
        {
        case Routine::SchemeFunction:
          {
            // delete the function.
            delete this->function;

            break;
          }
        case Routine::SchemeMethod:
          {
            // delete the method.
            delete this->method;

            break;
          }
        case Routine::SchemeUnknown:
          {
            break;
          }
        }
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method calls the callback implementation.
    ///
    template <typename R,
              typename... T>
    R
    Callback< R, Parameters<T...> >::Call(T...                  arguments)
      const
    {
      // call depending on the scheme.
      switch (this->scheme)
        {
        case Routine::SchemeFunction:
          {
            // call the function.
            if (this->function->Call(arguments...) == StatusError)
              escape("unable to call the function");

            break;
          }
        case Routine::SchemeMethod:
          {
            // call the method.
            if (this->method->Call(arguments...) == StatusError)
              escape("unable to call the method");

            break;
          }
        case Routine::SchemeUnknown:
          {
            break;
          }
        }

      return elle::StatusOk;
    }

    ///
    /// this method triggers the callback implementation without checking
    /// the return value.
    ///
    template <typename R,
              typename... T>
    Void
    Callback< R, Parameters<T...> >::Trigger(T...               arguments)
      const
    {
      // call depending on the scheme.
      switch (this->scheme)
        {
        case Routine::SchemeFunction:
          {
            // triggerl the function.
            this->function->Call(arguments...);

            break;
          }
        case Routine::SchemeMethod:
          {
            // trigger the method.
            this->method->Call(arguments...);

            break;
          }
        case Routine::SchemeUnknown:
          {
            break;
          }
        }
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(_(Callback< R, Parameters<T...> >),
          _(template <typename R,
                      typename... T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the callback.
    ///
    template <typename R,
              typename... T>
    Status
    Callback< R, Parameters<T...> >::Dump(const Natural32       margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Callback]" << std::endl;

      // dump the routine.
      if (Routine::Dump(margin + 2) == StatusError)
        escape("unable to dump the routine");

      // dump the content.
      switch (this->scheme)
        {
        case Routine::SchemeFunction:
          {
            // dump the function.
            if (this->function->Dump(margin + 2) == StatusError)
              escape("unable to dump the function");

            break;
          }
        case Routine::SchemeMethod:
          {
            // dump the method.
            if (this->method->Dump(margin + 2) == StatusError)
              escape("unable to dump the method");

            break;
          }
        case Routine::SchemeUnknown:
          {
            break;
          }
        }

      return elle::StatusOk;
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns a callback of the type of the given function.
    ///
    template <typename R,
              typename... T>
    Callback< R, Parameters<T...> >
    Callback<>::Infer(R                                 (*handler)(T...))
    {
      return (Callback< R, Parameters<T...> >(handler));
    }

    ///
    /// this method returns a callback of the type of the given method.
    ///
    template <typename R,
              typename C,
              typename... T>
    Callback< R, Parameters<T...> >
    Callback<>::Infer(R                                 (C::*handler)(T...),
                      C*                                object)
    {
      return (Callback< R, Parameters<T...> >(handler, object));
    }

  }
}

#endif
