#ifndef ELLE_CONCURRENCY_CLOSURE_HH
# define ELLE_CONCURRENCY_CLOSURE_HH

# include <elle/types.hh>

# include <elle/radix/Entity.hh>
# include <elle/radix/Parameters.hh>
# include <elle/radix/Arguments.hh>

# include <elle/concurrency/Callback.hh>

namespace elle
{
  namespace concurrency
  {
    ///
    /// Closure generic class.
    ///
    /// note that this class is specialized in either a static or dynamic
    /// closure.
    ///
    template <typename... T>
    class Closure;

    ///
    /// this specialized class provides static closures i.e static arguments
    /// bound to a callback.
    ///
    /// once triggered, the arguments attached to the closure are passed
    /// to the callback.
    ///
    template <typename R,
              typename... U>
    class Closure< R,
                   radix::Parameters<U...> >:
      public radix::Entity
    {
    public:
      //
      // types
      //
      typedef radix::Parameters<U...>          P;

      //
      // constructors & destructors
      //
      Closure(Callback< R, radix::Parameters<U...> >,
              U&...);

      //
      // methods
      //
      R                 Call();
      Void              Trigger();

      //
      // interfaces
      //

      // object
      declare(_(Closure< R, radix::Parameters<U...> >));

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Callback< R, radix::Parameters<U...> >   callback;
      radix::Arguments< radix::Parameters<U...> >     arguments;
    };

    ///
    /// this specialized class provides dynamic closures. unlike static
    /// closures, both static and dynamic arguments can be provided.
    ///
    /// for instance, the closure:
    ///
    ///   Closure< Parameters<Natural32>,
    ///            Parameters<String> > closure(callback, n);
    ///
    /// takes a Natural32 (n) as a static argument i.e at the closure creation
    /// but also takes a String once the closure is triggered. thus,
    /// such a closure would be used as follows:
    ///
    ///   closure.Call(string);
    ///   closure.Trigger(string);
    ///
    /// such a call would result in the callback being triggered with both
    /// the static and dynamic arguments. in this example, the callback
    /// would have to be of the type:
    ///
    ///   Callback< Parameters<Natural32, String> >
    ///
    template <typename R,
              typename... U,
              typename... V>
    class Closure< R,
                   radix::Parameters<U...>,
                   radix::Parameters<V...> >:
      public radix::Entity
    {
    public:
      //
      // types
      //
      typedef radix::Parameters<U..., V...>    P;

      //
      // constructors & destructors
      //
      Closure(Callback< R, radix::Parameters<U..., V...> >,
              U&...);

      //
      // methods
      //
      R                 Call(V&...);
      Void              Trigger(V&...);

      //
      // interfaces
      //

      // object
      declare(_(Closure< R, radix::Parameters<U...>, radix::Parameters<V...> >));

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Callback< R, radix::Parameters<U..., V...> >     callback;
      radix::Arguments< radix::Parameters<U...> >             arguments;
    };

    ///
    /// a specific class for closure inference.
    ///
    template <>
    class Closure<>
    {
    public:
      //
      // static methods
      //
      template <typename R,
                typename... U>
      static Closure< R,
                      radix::Parameters<U...> >        Infer(R (*)(U...));
      template <typename R,
                typename C,
                typename... U>
      static Closure< R,
                      radix::Parameters<U...> >        Infer(R (C::*)(U...),
                                                      C*);

      template <typename R,
                typename... U,
                typename... V>
      static Closure< R,
                      radix::Parameters<U...>,
                      radix::Parameters<V...> >        Infer(R (*)(U..., V...),
                                                      U&...);
      template <typename R,
                typename C,
                typename... U,
                typename... V>
      static Closure< R,
                      radix::Parameters<U...>,
                      radix::Parameters<V...> >        Infer(R (C::*)(U..., V...),
                                                      C*,
                                                      U&...);
    };

  }
}

#include <elle/concurrency/Closure.hxx>

#endif
