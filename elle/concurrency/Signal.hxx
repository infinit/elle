#ifndef ELLE_CONCURRENCY_SIGNAL_HXX
# define ELLE_CONCURRENCY_SIGNAL_HXX

# include <elle/standalone/Log.hh>
# include <elle/concurrency/Scheduler.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- signal ----------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename... T>
    Signal< radix::Parameters<T...> >::Signal():
      counter(0)
    {
    }

    ///
    /// destructor.
    ///
    template <typename... T>
    Signal< radix::Parameters<T...> >::~Signal()
    {
      // flush the signal.
      this->Flush();
    }

    ///
    /// this method subscribes the given object---callback, closure, etc.---
    /// without returning the associated stream identifier.
    ///
    template <typename... T>
    template <template <typename...> class C>
    Status
    Signal< radix::Parameters<T...> >::Subscribe(const C<
                                            Status,
                                            radix::Parameters<T...>
                                            >                   object)
    {
      Stream            useless;

      return (this->Subscribe(object, useless));
    }

    ///
    /// this method subscribes the given object along with returning the
    /// associated stream identifier.
    ///
    template <typename... T>
    template <template <typename...> class C>
    Status
    Signal< radix::Parameters<T...> >::Subscribe(const C<
                                            Status,
                                            radix::Parameters<T...>
                                            >                   object,
                                          Stream&               stream)
    {
      typedef Selectionoid<C<Status, radix::Parameters<T...>>> OID;
      typedef Signal<radix::Parameters<T...>>::Stream Stream;
      typedef Signal< radix::Parameters<T...>>::Functionoid Functionoid;
      typedef std::pair<Stream const , Functionoid*> ValueType;

      // increment the counter and set the stream.
      stream = ++this->counter;

      // create a new selectionoid.
      auto selectionoid = new OID(object);

      // insert the selectionoid in the container.
      assert(selectionoid);
      auto result = this->container.insert(
          ValueType(this->counter, selectionoid)
      );

      // check if the insertion was successful.
      if (result.second == false)
        {
          delete selectionoid;
          escape("unable to insert the selectoinoid in the container");
        }

      return Status::Ok;
    }

    ///
    /// this method unsubscribes the object associated with the given stream.
    ///
    template <typename... T>
    Status
    Signal< radix::Parameters<T...> >::Unsubscribe(const Stream        stream)
    {
      Signal< radix::Parameters<T...> >::Functionoid*  functionoid;
      Signal< radix::Parameters<T...> >::Iterator      iterator;

      // locate the functionoid.
      if ((iterator = this->container.find(stream)) != this->container.end())
        escape("unable to locate the given stream");

      // retrieve the functionoid.
      functionoid = iterator->second;

      // delete the functionoid.
      delete functionoid;

      // remove the functionoid from the container.
      this->container.erase(iterator);

      return Status::Ok;
    }

    ///
    /// this method emits the signal by calling all the subscribed objects.
    ///
    template <typename... T>
    Status
    Signal< radix::Parameters<T...> >::Emit(T...                       arguments)
    {
      Signal< radix::Parameters<T...> >::Scoutor       scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
      {
        Signal< radix::Parameters<T...> >::Functionoid*      functionoid =
          scoutor->second;
        EmitOne(functionoid, arguments...);
      }

      return Status::Ok;
    }

    template <typename... T>
    Status
    Signal< radix::Parameters<T...> >::AsyncEmit(T... arguments)
    {
      Signal< radix::Parameters<T...> >::Scoutor scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
      {
        Signal< radix::Parameters<T...> >::Functionoid* f = scoutor->second;
        new reactor::Thread(scheduler(), "Signal emission",
                            boost::bind(&Signal< radix::Parameters<T...> >::EmitOne,
                                        f, arguments...), true);
      }

      return Status::Ok;
    }


    template <typename... T>
    Status
    Signal< radix::Parameters<T...> >::EmitOne(
      Signal< radix::Parameters<T...> >::Functionoid* f,
      T... arguments)
    {
      assert(f);
      if (f->Call(arguments...) == Status::Error)
        log("an error occured while processing a signal");
      return Status::Ok;
    }

    ///
    /// this method flushes the subscribed objects associated with the signal.
    ///
    template <typename... T>
    Status
    Signal< radix::Parameters<T...> >::Flush()
    {
      Signal< radix::Parameters<T...> >::Scoutor       scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          Signal< radix::Parameters<T...> >::Functionoid*      functionoid =
            scoutor->second;

          // delete the functionoid.
          delete functionoid;
        }

      // clear the container.
      this->container.clear();

      return Status::Ok;
    }

    ///
    /// this method dumps the signal.
    ///
    template <typename... T>
    Status
    Signal< radix::Parameters<T...> >::Dump(const Natural32            margin) const
    {
      String            alignment(margin, ' ');
      auto              iterator = this->container.begin();
      auto              end = this->container.end();

      std::cout << alignment << "[Signal]" << std::endl;

      // dump the mode.
      std::cout << alignment << io::Dumpable::Shift
                << "[Functionoids] " << this->container.size() << std::endl;

      return Status::Ok;
    }

    template <typename ...T>
    template <typename Y>
    typename Signal< radix::Parameters<T...> >::Functionoid*
    Signal< radix::Parameters<T...> >::Selectionoid<Y>::clone() const
    {
      return new Self(*this);
    }

//
// ---------- selectonoid -----------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename... T>
    template <typename Y>
    Signal< radix::Parameters<T...> >::
     Selectionoid<Y>::Selectionoid(const Y&                     object):
      object(object)
    {
    }

    ///
    /// this method forwards the request by calling the object.
    ///
    template <typename... T>
    template <typename Y>
    Status
    Signal< radix::Parameters<T...> >::
     Selectionoid<Y>::Call(T&...                                arguments)
      const
    {
      return (this->object.Call(arguments...));
    }

  }
}

#endif
