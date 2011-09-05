//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Signal.hxx
//
// created       julien quintard   [fri sep  2 14:12:04 2011]
// updated       julien quintard   [sat sep  3 23:37:39 2011]
//

#ifndef ELLE_CONCURRENCY_SIGNAL_HXX
#define ELLE_CONCURRENCY_SIGNAL_HXX

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
    Signal< Parameters<T...> >::Signal():
      counter(0)
    {
    }

    ///
    /// destructor.
    ///
    template <typename... T>
    Signal< Parameters<T...> >::~Signal()
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
    Signal< Parameters<T...> >::Subscribe(const C<
					    Status,
					    Parameters<T...>
					    >			object)
    {
      Stream		useless;

      return (this->Subscribe(object, useless));
    }

    ///
    /// this method subscribes the given object along with returning the
    /// associated stream identifier.
    ///
    template <typename... T>
    template <template <typename...> class C>
    Status
    Signal< Parameters<T...> >::Subscribe(const C<
					    Status,
					    Parameters<T...>
					    >			object,
					  Stream&		stream)
    {
      Selectionoid<
	C<
	  Status,
	  Parameters<T...>
	  >
	>*			selectionoid;
      std::pair<
	Signal<
	  Parameters<T...>
	  >::Iterator,
	Boolean
	>			result;

      enter(instance(selectionoid));

      // increment the counter and set the stream.
      stream = ++this->counter;

      // create a new selectionoid.
      selectionoid =
	new Selectionoid<
	  C<
	    Status,
	    Parameters<T...>
	    >
	  >(object);

      // insert the selectionoid in the container.
      result = this->container.insert(
	         std::pair<const Signal<
		             Parameters<T...>
		             >::Stream,
			   Signal<
			     Parameters<T...>
			     >::Functionoid*>(this->counter,
		                              selectionoid));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the selectoinoid in the container");

      // waive the selectionoid tracking.
      waive(selectionoid);

      leave();
    }

    ///
    /// this method unsubscribes the object associated with the given stream.
    ///
    template <typename... T>
    Status
    Signal< Parameters<T...> >::Unsubscribe(const Stream	stream)
    {
      Signal< Parameters<T...> >::Functionoid*	functionoid;
      Signal< Parameters<T...> >::Iterator	iterator;

      enter();

      // locate the functionoid.
      if ((iterator = this->container.find(stream)) != this->container.end())
	escape("unable to locate the given stream");

      // retrieve the functionoid.
      functionoid = iterator->second;

      // delete the functionoid.
      delete functionoid;

      // remove the functionoid from the container.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method emits the signal by calling all the subscribed objects.
    ///
    template <typename... T>
    Status
    Signal< Parameters<T...> >::Emit(T...			arguments)
    {
      Signal< Parameters<T...> >::Scoutor	scoutor;

      enter();

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Signal< Parameters<T...> >::Functionoid*	functionoid =
	    scoutor->second;

	  // call the functionoid's object.
	  if (functionoid->Call(arguments...) == StatusError)
	    {
	      // if an error occured, log it.
	      log("an error occured while processing a signal");
	    }
	}

      leave();
    }

    ///
    /// this method flushes the subscribed objects associated with the signal.
    ///
    template <typename... T>
    Status
    Signal< Parameters<T...> >::Flush()
    {
      Signal< Parameters<T...> >::Scoutor	scoutor;

      enter();

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Signal< Parameters<T...> >::Functionoid*	functionoid =
	    scoutor->second;

	  // delete the functionoid.
	  delete functionoid;
	}

      // clear the container.
      this->container.clear();

      leave();
    }

//
// ---------- selectonoid -----------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename... T>
    template <typename Y>
    Signal< Parameters<T...> >::
     Selectionoid<Y>::Selectionoid(const Y&			object):
      object(object)
    {
    }

    ///
    /// this method forwards the request by calling the object.
    ///
    template <typename... T>
    template <typename Y>
    Status
    Signal< Parameters<T...> >::
     Selectionoid<Y>::Call(T&...				arguments)
      const
    {
      return (this->object.Call(arguments...));
    }

    ///
    /// this method dumps the selectionoid.
    ///
    template <typename... T>
    template <typename Y>
    Status
    Signal< Parameters<T...> >::
     Selectionoid<Y>::Dump(const Natural32			margin) const
    {
      enter();

      // dump the object.
      if (this->object.Dump(margin) == StatusError)
	escape("unable to dump the object");

      leave();
    }

  }
}

#endif
