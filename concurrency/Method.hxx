#ifndef ELLE_CONCURRENCY_METHOD_HXX
# define ELLE_CONCURRENCY_METHOD_HXX

# include <elle/types.hh>

# include <elle/standalone/Report.hh>
 
namespace elle
{
  namespace concurrency
  {

//
// ---------- wrap ------------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename R,
              typename... T>
    template <typename C>
    Method< R, radix::Parameters<T...> >::Wrap<C>::Wrap(Handler        handler,
                                                 C*             object):
      object(object),
      handler(handler)
    {
    }

    ///
    /// this method calls the handler.
    ///
    template <typename R,
              typename... T>
    template <typename C>
    R
    Method< R, radix::Parameters<T...> >::Wrap<C>::Call(T...           arguments)
    {
      return ((this->object->*this->handler)(arguments...));
    }

    ///
    /// this method triggers the handler without checking the return value.
    ///
    template <typename R,
              typename... T>
    template <typename C>
    Void
    Method< R, radix::Parameters<T...> >::Wrap<C>::Trigger(T...        arguments)
    {
      (this->object->*this->handler)(arguments...);
    }

    ///
    /// this method dumps the method state.
    ///
    template <typename R,
              typename... T>
    template <typename C>
    Status
    Method< R, radix::Parameters<T...> >::Wrap<C>::Dump(const Natural32 margin)
      const
    {
      String            alignment(margin, ' ');

      // dump the quantum.
      std::cout << alignment << io::Dumpable::Shift << "[Quantum] "
                << std::dec << sizeof... (T) << std::endl;

      // dump the object.
      std::cout << alignment << io::Dumpable::Shift << "[Object] "
                << std::hex << this->object << std::endl;

      // dump the handler.
      std::cout << alignment << io::Dumpable::Shift << "[Handler] "
                << std::hex << this->handler << std::endl;

      return Status::Ok;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(_(Method< R, radix::Parameters<T...> >::Wrap<C>),
          _(template <typename R,
                      typename... T>
            template <typename C>));

//
// ---------- method ----------------------------------------------------------
//

    ///
    /// the default constructor.
    ///
    template <typename R,
              typename... T>
    template <typename C>
    Method< R, radix::Parameters<T...> >::Method(R             (C::*handler)(T...),
                                          C*            object):
      shell(new Method< R, radix::Parameters<T...> >::Wrap<C>(handler, object))
    {
    }

    ///
    /// the copy constructor.
    ///
    template <typename R,
              typename... T>
    Method< R, radix::Parameters<T...> >::Method(
                                     const
                                       Method<
                                         R,
                                         radix::Parameters<T...>
                                         >&                     method):
      Object(method)
    {
      // clone the shell.
      //
      // note that the Clone method must be called because at this point,
      // this constructor does not know the type of the _method_'s wrap.
      if (method.shell != NULL)
        method.shell->Clone(reinterpret_cast<Object*&>(this->shell));
    }

    ///
    /// destructor.
    ///
    template <typename R,
              typename... T>
    Method< R, radix::Parameters<T...> >::~Method()
    {
      // delete the shell, if present.
      if (this->shell != NULL)
        delete this->shell;
    }

    ///
    /// this method calls the handler.
    ///
    template <typename R,
              typename... T>
    R
    Method< R, radix::Parameters<T...> >::Call(T...                    arguments)
    {
      return (this->shell->Call(arguments...));
    }

    ///
    /// this method triggers the handler without checking the return value.
    ///
    template <typename R,
              typename... T>
    Void
    Method< R, radix::Parameters<T...> >::Trigger(T...                 arguments)
    {
      this->shell->Trigger(arguments...);
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// these are generated automatically.
    ///
    embed(_(Method< R, radix::Parameters<T...> >),
          _(template <typename R,
                      typename... T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the method state.
    ///
    template <typename R,
              typename... T>
    Status
    Method< R, radix::Parameters<T...> >::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Method]" << std::endl;

      // dump the shell.
      if (this->shell->Dump(margin) == Status::Error)
        escape("unable to dump the shell");

      return Status::Ok;
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns a method of the type of the given object/handler.
    ///
    template <typename R,
              typename C,
              typename... T>
    Method< R, radix::Parameters<T...> >
    Method<>::Infer(R                                   (C::*handler)(T...),
                    C*                                  object)
    {
      return (Method< R, radix::Parameters<T...> >(handler, object));
    }

  }
}

#endif
