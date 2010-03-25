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
// updated       julien quintard   [thu mar 25 01:23:32 2010]
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
    Void		Fiber::Trigger(Entrance*		entrance,
				       T*...			arguments)
    {
      enter();

      // trigger the entrance and store the return status.
      Fiber::Current->status = entrance->Trigger(arguments...);

      // set the fiber state.
      Fiber::Current->state = Fiber::StateCompleted;
    }

    ///
    /// this method spawns a fiber.
    ///
    template <typename... T>
    Status		Fiber::Spawn(Entrance*			entrance,
				     T*...			arguments)
    {
      enter();

      printf("[XXX 0x%x] Fiber::Spawn()\n", Fiber::Current);

      // check if the current fiber is different from the application,
      // which would be an error.
      if (Fiber::Current != Fiber::Application)
	escape("it seems that the current fiber is not the application");

      // get the current context.
      if (::getcontext(&Fiber::Current->context) == -1)
	escape("unable to get the context");

      printf("[XXX 0x%x] Fiber::Spawn(%u)\n",
	     Fiber::Current, Fiber::Current->state);

      // if we are still in the application, spawn the even fiber.
      if (Fiber::Current == Fiber::Application)
	{
	  printf("[XXX] Fiber::Spawn() :: Application\n");

	  // declare a trigger function pointer in order to bypass the type
	  // checking system through casts.
	  Void		(*trigger)(Entrance*, T*...) = &Fiber::Trigger;

	  // get the current context in order to create a new one.
	  if (::getcontext(&Fiber::Handler->context) == -1)
	    escape("unable to get the context");

	  // modify the context manually.
	  Fiber::Handler->context.uc_link =
	    &Fiber::Application->context;
	  Fiber::Handler->context.uc_stack.ss_sp =
	    Fiber::Handler->frame->stack;
	  Fiber::Handler->context.uc_stack.ss_size =
	    Fiber::Handler->frame->size;
	  Fiber::Handler->context.uc_flags = 0;

	  // set the current fiber.
	  Fiber::Current = Fiber::Handler;

	  // create a context for the event fiber, with the Fiber::Trigger
	  // as entry point.
	  ::makecontext(&Fiber::Handler->context,
			(void (*)())trigger,
			1 + sizeof...(T),
			entrance, arguments...);

	  // set the fiber state.
	  Fiber::Handler->state = Fiber::StateActive;

	  // set the new context.
	  if (::setcontext(&Fiber::Handler->context) == -1)
	    escape("unable to set the context");

	  //
	  // should never reach this point since ::setcontext() never
	  // returns.
	  //
	  release();
	  fail("this code should never have been reached");
	}

      printf("[XXX 0x%x] Fiber::Spawn() :: Handler\n", Fiber::Current);

      //
      // at this point, we just came back from a fiber.
      //
      Status		status;

      // retrieve the status from the fiber.
      status = Fiber::Current->status;

      // perform an action depending on the state of the fiber.
      switch (Fiber::Current->state)
	{
	case Fiber::StateCompleted:
	  {
	    // if the fiber has completed, delete it, unless it is the
	    // event fiber in which case it will be re-used for the next
	    // event to process.
	    if (Fiber::Current != Fiber::Handler)
	      {
		// remove from the frames.
		if (Fiber::Remove(Fiber::Current) == StatusError)
		  escape("unable to remove the current fiber");

		// delete the fiber.
		delete Fiber::Current;
	      }
	    else
	      {
		// otherwise, re-set the state and status to unknown so that
		// the event fiber can be re-used.
		Fiber::Current->state = StateUnknown;
		Fiber::Current->status = StatusUnknown;
	      }

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
	    escape("at this pointer a fiber cannot be in an unknown or "
		   "active state");
	  }
	}

      // set the current fiber as being the application.
      Fiber::Current = Fiber::Application;

      // schedule the awaken fibers only if the status to return to
      // the caller is not an error. if it is, the system must inform the
      // caller and the awaken fibers are therefore not scheduled but will
      // probably be next time.
      if (status != StatusError)
	if (Fiber::Schedule() == StatusError)
	  escape("unable to schedule the awaken fibers");

      printf("[XXX] Fiber::Spawn() :: %u\n", status);

      release();
      return (status);
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

      printf("[XXX] Fiber::Wait() 0x%x\n", Fiber::Current);

      // check if the current fiber is the handler.
      if (Fiber::Current != Fiber::Handler)
	escape("it seems that the current fiber is not the handler");

      // set the event.
      Fiber::Current->event = event;

      // set the fiber has been suspended.
      Fiber::Current->state = Fiber::StateSuspended;

      // allocate a new fiber, replacing the handler.
      Fiber::Handler = new Fiber;

      // create the fiber.
      if (Fiber::Handler->Create() == StatusError)
	escape("unable to create the new handler");

      // add the current fiber to the container since it is, from now
      // on, waiting on an event.
      if (Fiber::Add(Fiber::Current) == StatusError)
	escape("unable to add the new handler's fiber to the container");

      // switch to the application's context and save the current one
      // in order to carry on at this point when woken up.
      if (::swapcontext(&Fiber::Current->context,
			&Fiber::Application->context) == -1)
	escape("unable to swap to the application context");

      // set the data.
      data = (T*)Fiber::Current->data;

      printf("[/XXX] Fiber::Wait() 0x%x\n", Fiber::Current);

      leave();
    }

    ///
    /// this method wakes up the fibers waiting for the given event.
    ///
    template <typename T>
    Status		Fiber::Awaken(const Event&		event,
				      T*			data)
    {
      Fiber::Iterator	iterator;

      enter();

      // check if there are blocked fibers.
      if (Fiber::Fibers.empty() == true)
	leave();

      // check if the current fiber is the handler.
      if (Fiber::Current != Fiber::Handler)
	escape("it seems that the current fiber is not the handler");

      // manually go through the fibers and awaken all the fibers
      // expecting the given event.
      for (iterator = Fiber::Fibers.begin();
	   iterator != Fiber::Fibers.end();
	   iterator++)
	{
	  Fiber*	fiber = *iterator;

	  // check if this fiber is waiting for the given event.
	  if (event == fiber->event)
	    {
	      // set the data.
	      fiber->data = (Void*)data;

	      // set the state as Awaken.
	      fiber->state = Fiber::StateAwaken;

	      // unset the event.
	      fiber->event = Event::Null;
	    }
	}

      leave();
    }

  }
}

#endif
