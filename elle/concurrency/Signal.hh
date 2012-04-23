#ifndef ELLE_CONCURRENCY_SIGNAL_HH
# define ELLE_CONCURRENCY_SIGNAL_HH

#  include <map>

# include <elle/types.hh>

# include <elle/radix/Parameters.hh>
# include <elle/radix/Object.hh>
# include <elle/radix/Entity.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the generic Signal.
    ///
    template <typename... T>
    class Signal;

    ///
    /// a signal represents an event to which one can subscribe so that
    /// when it is Q_EMITted, the subscribed callbacks are called.
    ///
    template <typename... T>
    class Signal< Parameters<T...> >:
      public elle::radix::Object
    {
    public:
      //
      // types
      //
      typedef Parameters<T...>          P;

      ///
      /// this class represents the functionoid used to forward the call.
      ///
      class Functionoid:
        public Entity
      {
      public:
        virtual Functionoid* clone() const = 0;

        //
        // constructors & destructors
        //
        virtual ~Functionoid()
        {
        }

        //
        // methods
        //
        virtual Status  Call(T&...) const = 0;
      };

      ///
      /// this implementation takes a set of arguments and triggers the
      /// subscribed object.
      ///
      template <typename Y>
      class Selectionoid:
        public Functionoid
      {
      public:
        typedef Selectionoid<Y> Self;
        virtual Functionoid* clone() const;

        //
        // constructors & destructors
        //
        Selectionoid(const Y&);

        //
        // methods
        //
        Status          Call(T&...) const;

        //
        // attributes
        //
        Y               object;
      };

      //
      // types
      //
      typedef Natural32                                 Stream;

      typedef std::map<const Natural32,
                       Functionoid*>                    Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // constructors & destructors
      //
      Signal();
      ~Signal();

      //
      // methods
      //
      template <template <typename...> class C>
      Status            Subscribe(const C<Status, P>);
      template <template <typename...> class C>
      Status            Subscribe(const C<Status, P>,
                                  Stream&);
      Status            Unsubscribe(const Stream);
      Status            Emit(T...);
      Status            AsyncEmit(T...);
      static
      Status            EmitOne(Signal< Parameters<T...> >::Functionoid* f,
                                T...);

      Status            Flush();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Stream            counter;

      Container         container;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Signal.hxx>

#endif
