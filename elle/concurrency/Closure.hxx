#ifndef ELLE_CONCURRENCY_CLOSURE_HXX
# define ELLE_CONCURRENCY_CLOSURE_HXX

namespace elle
{
  namespace concurrency
  {

//
// ---------- static closure --------------------------------------------------
//

    ///
    /// the constructor.
    ///
    template <typename R,
              typename... U>
    Closure< R, radix::Parameters<U...> >::Closure(
        Callback< R, radix::Parameters<U...> >                         callback,
        U&...                                                   objects):
      callback(callback),
      arguments(objects...)
    {
    }

    ///
    /// this method calls the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename R,
              typename... U>
    R           Closure< R, radix::Parameters<U...> >::Call()
    {
      return (this->arguments.Call(this->callback));
    }

    ///
    /// this method triggers the closure without checking the return value.
    ///
    template <typename R,
              typename... U>
    Void        Closure< R, radix::Parameters<U...> >::Trigger()
    {
      this->arguments.Trigger(this->callback);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(_(Closure< R, radix::Parameters<U...> >),
          _(template <typename R,
                      typename... U>));

    ///
    /// this method dumps the closure.
    ///
    template <typename R,
              typename... U>
    Status
    Closure< R, radix::Parameters<U...> >::Dump(const Natural32        margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Closure]" << std::endl;

      // dump the callback.
      if (this->callback.Dump(margin + 2) == Status::Error)
        escape("unable to dump the callback");

      // dump the arguments.
      if (this->arguments.Dump(margin + 2) == Status::Error)
        escape("unable to dump the arguments");

      return Status::Ok;
    }

//
// ---------- dynamic closure -------------------------------------------------
//

    ///
    /// the constructor.
    ///
    template <typename R,
              typename... U,
              typename... V>
    Closure< R,
             radix::Parameters<U...>,
             radix::Parameters<V...> >::Closure(
        Callback< R, radix::Parameters<U..., V...> >                   callback,
        U&...                                                   objects):
      callback(callback),
      arguments(objects...)
    {
    }

    ///
    /// this method calls the closure by calling the callback with
    /// the pre-defined arguments.
    ///
    template <typename R,
              typename... U,
              typename... V>
    R           Closure< R,
                         radix::Parameters<U...>,
                         radix::Parameters<V...> >::Call(V&...         objects)
    {
      return (this->arguments.Trigger(this->callback, objects...));
    }

    ///
    /// this method triggers the closure without checking the return value.
    ///
    template <typename R,
              typename... U,
              typename... V>
    Void        Closure< R,
                         radix::Parameters<U...>,
                         radix::Parameters<V...> >::Trigger(V&...      objects)
    {
      this->arguments.Trigger(this->callback, objects...);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(_(Closure< R, radix::Parameters<U...>, radix::Parameters<V...> >),
          _(template <typename R,
                      typename... U,
                      typename... V>));

    ///
    /// this method dumps the closure.
    ///
    template <typename R,
              typename... U,
              typename... V>
    Status
    Closure< R,
             radix::Parameters<U...>,
             radix::Parameters<V...> >::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Closure]" << std::endl;

      // dump the callback.
      if (this->callback.Dump(margin + 2) == Status::Error)
        escape("unable to dump the callback");

      // dump the arguments.
      if (this->arguments.Dump(margin + 2) == Status::Error)
        escape("unable to dump the arguments");

      return Status::Ok;
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns a closure of the type of the given function.
    ///
    template <typename R,
              typename... U>
    Closure< R, radix::Parameters<U...> >
    Closure<>::Infer(R                                  (*handler)(U...))
    {
      return (Closure< R, radix::Parameters<U...> >(
                Callback<>::Infer(handler)));
    }

    ///
    /// this method returns a closure of the type of the given method.
    ///
    template <typename R,
              typename C,
              typename... U>
    Closure< R, radix::Parameters<U...> >
    Closure<>::Infer(R                                  (C::*handler)(U...),
                     C*                                 object)
    {
      return (Closure< R, radix::Parameters<U...> >(
                Callback<>::Infer(handler, object)));
    }

    ///
    /// this method returns a closure of the type of the given function, plus
    /// initial arguments.
    ///
    template <typename R,
              typename... U,
              typename... V>
    Closure< R, radix::Parameters<U...>, radix::Parameters<V...> >
    Closure<>::Infer(R                                  (*handler)(U...,
                                                                   V...),
                     U&...                              arguments)
    {
      return (Closure< R, radix::Parameters<U...>, radix::Parameters<V...> >(
                Callback<>::Infer(handler), arguments...));
    }

    ///
    /// this method returns a closure of the type of the given method, plus
    /// initial arguments.
    ///
    template <typename R,
              typename C,
              typename... U,
              typename... V>
    Closure< R, radix::Parameters<U...>, radix::Parameters<V...> >
    Closure<>::Infer(R                                  (C::*handler)(U...,
                                                                      V...),
                     C*                                 object,
                     U&...                              arguments)
    {
      return (Closure< R, radix::Parameters<U...>, radix::Parameters<V...> >(
                Callback<>::Infer(handler, object), arguments...));
    }

  }
}

#endif
