//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Fiber.hxx
//
// created       julien quintard   [tue mar 23 14:55:13 2010]
// updated       julien quintard   [sun mar 28 23:43:25 2010]
//

#ifndef ELLE_CONCURRENCY_FIBER_HXX
#define ELLE_CONCURRENCY_FIBER_HXX

namespace elle
{
  namespace concurrency
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method wraps a fiber entry point setting the fiber's state
    /// in order to distinguish the thread returning from ::getcontext()
    /// and jumping back.
    ///
    template <typename... T>
    Void		Fiber::Trigger(Closure<T...>*		closure)
    {
      enter();

      // trigger the closure and store the return status.
      Fiber::Current->status = closure->Call();

      // set the fiber state.
      Fiber::Current->state = Fiber::StateCompleted;

      // remove the parent fiber from the container since it is going
      // to be scheduled as soon as this function returns.
      if (Fiber::Remove(Fiber::Current->link) == StatusError)
	alert("unable to remove the fiber");

      // set the state of the parent's fiber as awaken.
      Fiber::Current->link->state = Fiber::StateAwaken;
    }

    ///
    /// this method spawns a fiber.
    ///
    template <typename... T>
    Status		Fiber::Spawn(Closure<T...>&		closure)
    {
      enter();

      //printf("[XXX 0x%x] Fiber::Spawn()\n", Fiber::Current);

      // get the current context in order to resume execution for this point
      // in the future.
      if (::getcontext(&Fiber::Current->context) == -1)
	escape("unable to get the context");

      //printf("[XXX 0x%x] Fiber::Spawn(%u)\n",
      //Fiber::Current, Fiber::Current->state);
      //Fiber::Show();

      // if we are in the fiber spawning a new fiber, the parent fiber
      // some might say.
      if (Fiber::Current->state == Fiber::StateActive)
	{
	  // declare a trigger function pointer in order to bypass the type
	  // checking system through casts.
	  Void		(*trigger)(Closure<T...>*) = &Fiber::Trigger;
	  Fiber*	fiber;

	  //printf("[XXX] Fiber::Spawn() :: Parent\n");

	  // set the current fiber as suspended.
	  Fiber::Current->state = Fiber::StateSuspended;

	  // the current fiber is waiting for its child fiber to complete.
	  Fiber::Current->type = Fiber::TypeFiber;

	  // add the current fiber to the container.
	  if (Fiber::Add(Fiber::Current) == StatusError)
	    escape("unable to add the fiber to the container");

	  // allocate a new fiber.
	  if (Fiber::New(fiber) == StatusError)
	    escape("unable to allocate a new fiber");

	  // get the context in order to create a new one.
	  if (::getcontext(&fiber->context) == -1)
	    escape("unable to get the context");

	  // set the parent fiber.
	  fiber->link = Fiber::Current;

	  // modify the context manually so that, once completed, the
	  // execution comes back to the parent fiber i.e the current fiber.
	  fiber->context.uc_link = &fiber->link->context;
	  fiber->context.uc_stack.ss_sp = fiber->frame->stack;
	  fiber->context.uc_stack.ss_size = fiber->frame->size;
	  fiber->context.uc_flags = 0;

	  // create a context for the new fiber, with the Fiber::Trigger
	  // as entry point.
	  ::makecontext(&fiber->context,
			(void (*)())trigger,
			1,
			&closure);

	  // set the fiber state.
	  fiber->state = Fiber::StateActive;

	  // set the new fiber as the current one.
	  Fiber::Current = fiber;

	  // set the new context.
	  if (::setcontext(&Fiber::Current->context) == -1)
	    escape("unable to set the context");

	  //
	  // should never reach this point since ::setcontext() never
	  // returns.
	  //
	  release();
	  fail("this code should never have been reached");
	}
      else
	{
	  Status	status;
	  Fiber*	fiber;

	  //printf("[XXX 0x%x] Fiber::Spawn() :: Child\n",
	  //Fiber::Current);

	  //
	  // at this point, we just came back from a fiber.
	  //

	  // retrieve the link the the current's parent fiber.
	  fiber = Fiber::Current->link;

	  // retrieve the status from the fiber.
	  status = Fiber::Current->status;

	  // perform an action depending on the state of the fiber.
	  switch (Fiber::Current->state)
	    {
	    case Fiber::StateCompleted:
	      {
		// if the fiber has completed, delete it.
		if (Fiber::Delete(Fiber::Current) == StatusError)
		  escape("unable to delete the fiber");

		break;
	      }
	    case Fiber::StateSuspended:
	      {
		// do not delete this fiber as it will be resumed later.
		break;
	      }
	    case Fiber::StateActive:
	    case Fiber::StateUnknown:
	    default:
	      {
		escape("at this point a fiber cannot be in an unknown or "
		       "active state");
	      }
	    }

	  // set the current fiber as being the parent.
	  Fiber::Current = fiber;

	  // set the parent, now current, fiber as active.
	  Fiber::Current->state = Fiber::StateActive;

	  // schedule the awaken fibers only if the status to return to
	  // the caller is not an error. if it is, the system must inform the
	  // caller and the awaken fibers are therefore not scheduled but will
	  // probably be next time.
	  if (status != StatusError)
	    {
	      if (Fiber::Schedule() == StatusError)
		escape("unable to schedule the awaken fibers");
	    }

	  //printf("[/XXX] Fiber::Spawn() :: %u\n", status);
	  //Fiber::Show();

	  release();
	  return (status);
	}
    }

    ///
    /// this method takes the current fiber and sets the event it is
    /// expected to continue.
    ///
    template <typename T>
    Status		Fiber::Wait(const Event&		event,
				    T*&				data)
    {
      enter();

      // check if the current fiber is the application.
      if (Fiber::Current == Fiber::Application)
	escape("unable to wait while in the application fiber");

      //printf("[XXX 0x%x] Fiber::Wait(event[%qu])\n",
      //Fiber::Current, event.identifier);

      // set the fiber has been suspended.
      Fiber::Current->state = Fiber::StateSuspended;

      // set the type.
      Fiber::Current->type = Fiber::TypeEvent;

      // set the event.
      Fiber::Current->event = new Event(event);

      // add the current fiber to the container.
      if (Fiber::Add(Fiber::Current) == StatusError)
	escape("unable to add the fiber to the container");

      // set the state of the parent's fiber as awaken.
      Fiber::Current->link->state = Fiber::StateAwaken;

      // switch to the application's context and save the current one
      // in order to carry on at this point when woken up.
      if (::swapcontext(&Fiber::Current->context,
			&Fiber::Application->context) == -1)
	escape("unable to swap to the application context");

      // retrieve the data.
      data = (T*)Fiber::Current->data;

      //printf("[/XXX 0x%x] Fiber::Wait() :: 0x%x\n",
      //Fiber::Current, data);

      leave();
    }

    ///
    /// this method takes the current fiber and sets the resource it is
    /// expected to continue.
    ///
    template <typename T>
    Status		Fiber::Wait(const Resource*		resource,
				    T*&				data)
    {
      enter();

      // check if the current fiber is the application.
      if (Fiber::Current == Fiber::Application)
	escape("unable to wait while in the application fiber");

      //printf("[XXX 0x%x] Fiber::Wait(resource[0x%x])\n",
      //Fiber::Current, resource);

      // set the fiber has been suspended.
      Fiber::Current->state = Fiber::StateSuspended;

      // set the type.
      Fiber::Current->type = Fiber::TypeResource;

      // set the event.
      Fiber::Current->resource = resource;

      // add the current fiber to the container.
      if (Fiber::Add(Fiber::Current) == StatusError)
	escape("unable to add the fiber to the container");

      // set the state of the parent's fiber as awaken.
      Fiber::Current->link->state = Fiber::StateAwaken;

      // switch to the application's context and save the current one
      // in order to carry on at this point when woken up.
      if (::swapcontext(&Fiber::Current->context,
			&Fiber::Application->context) == -1)
	escape("unable to swap to the application context");

      // retrieve the data.
      data = (T*)Fiber::Current->data;

      //printf("[/XXX 0x%x] Fiber::Wait() :: 0x%x\n",
      //Fiber::Current, data);

      leave();
    }

    ///
    /// this method wakes up the fiber waiting for the given event.
    ///
    template <typename T>
    Status		Fiber::Awaken(const Event&		event,
				      T*			data)
    {
      Fiber::F::Iterator	iterator;
      Fiber*			fiber;
      Boolean			awaken;

      enter();

      // check if there are blocked fibers.
      if (Fiber::Fibers.empty() == true)
	false();

      // set the boolean to false meaning that no fiber has been woken up.
      awaken = false;

      // locate, awaken and remove fibers as long as found.
      while (Fiber::Locate(event, iterator) == true)
	{
	  Fiber*	fiber = *iterator;

	  // set the boolean to true.
	  awaken = true;

	  // set the data.
	  fiber->data = (Void*)data;

	  // set the state as awaken.
	  fiber->state = Fiber::StateAwaken;

	  // reset the type.
	  fiber->type = Fiber::TypeNone;

	  // delete the event.
	  delete fiber->event;
	  fiber->event = NULL;

	  //printf("[XXX 0x%x] Fiber::Awaken(event[%qu] data[0x%x])\n",
	  //Fiber::Current, event.identifier, data);
	}

      // return true if at least one fiber has been awaken.
      if (awaken == true)
	true();

      false();
    }

    ///
    /// this method wakes up the fibers waiting for the given resource.
    ///
    template <typename T>
    Status		Fiber::Awaken(const Resource*		resource,
				      T*			data)
    {
      Fiber::F::Iterator	iterator;
      Fiber*			fiber;
      Boolean			awaken;

      enter();

      // check if there are blocked fibers.
      if (Fiber::Fibers.empty() == true)
	false();

      // set the boolean to false meaning that no fiber has been woken up.
      awaken = false;

      // locate, awaken and remove fibers as long as found.
      while (Fiber::Locate(resource, iterator) == true)
	{
	  Fiber*	fiber = *iterator;

	  // set the boolean to true.
	  awaken = true;

	  // set the data.
	  fiber->data = (Void*)data;

	  // set the state as awaken.
	  fiber->state = Fiber::StateAwaken;

	  // reset the type.
	  fiber->type = Fiber::TypeNone;

	  //printf("[XXX 0x%x] Fiber::Awaken(resource[0x%x] data[0x%x])\n",
	  //Fiber::Current, resource, data);
	}

      // return true if at least one fiber has been awaken.
      if (awaken == true)
	true();

      false();
    }

  }
}

#endif
