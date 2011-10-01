//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [mon mar 22 02:22:43 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Morgue.hh>
#include <elle/concurrency/Fiber.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;

  namespace concurrency
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines fibers' stack size, in bytes: 1MB
    ///
    const Natural32		Fiber::Size = 2048576;

    ///
    /// this value defines the fibers cache capacity.
    ///
    const Natural32		Fiber::Capacity = 0;

    ///
    /// this variable holds the fibers waiting or that have been waiting
    /// for an event/resource.
    ///
    Fiber::F::Container		Fiber::Fibers;

    ///
    /// this fiber corresponds to the program just when entering
    /// the processing of an event.
    ///
    Fiber*			Fiber::Program = NULL;

    ///
    /// this variable points to the fiber currently in use.
    ///
    Fiber*			Fiber::Current = NULL;

    ///
    /// this variable holds fibers that are kept ready in case the
    /// system needs one. since most of the time, a single fiber is allocated
    /// every time an event is to be processed, keeping a single fiber in cache
    /// enables the system to exhibit good performance.
    ///
    Fiber::C::Container		Fiber::Cache;

    ///
    /// this variable is only used for callers not willing to receive
    /// data when awaken. in this case, this variable is used to receive
    /// the value instead. in other words this variable is unused but
    /// to receive rubbish.
    ///
    Meta*			Fiber::Trash = NULL;

    ///
    /// this container holds the callbacks to trigger whenever the state
    /// of a fiber is to be initialized, saved, restored or cleaned.
    ///
    Fiber::P::Container		Fiber::Phases;

    ///
    /// is the method Fiber::Schedule running ?
    ///
    bool                        Fiber::IsScheduling = false;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the fiber system.
    ///
    Status		Fiber::Initialize()
    {
      enter();

      // allocate the program fiber but do not create it since
      // this program has no need for a stack.
      Fiber::Program = new Fiber;

      // allocate an environment.
      Fiber::Program->environment = new Environment;

      // set the program as being running.
      Fiber::Program->state = Fiber::StateActive;

      // set the current fiber as being the program.
      Fiber::Current = Fiber::Program;

      leave();
    }

    ///
    /// this method cleans the fiber system.
    ///
    Status		Fiber::Clean()
    {
      enter();

      //
      // first, clean the phases callbacks.
      //
      {
        Fiber::P::Scoutor	scoutor;

	// go through the phases.
	for (scoutor = Fiber::Phases.begin();
	     scoutor != Fiber::Phases.end();
	     scoutor++)
	  {
	    Callback<
	      Status,
	      Parameters<Phase, Fiber*> >*	callback = *scoutor;

	    // delete the callback.
	    delete callback;
	  }
      }

      //
      // then, clean the cached fibers.
      //
      {
        Fiber::C::Scoutor	scoutor;

	// go through the cache.
	for (scoutor = Fiber::Cache.begin();
	     scoutor != Fiber::Cache.end();
	     scoutor++)
	  {
	    Fiber*	fiber = *scoutor;

	    // delete the fiber's environment.
	    if (fiber->environment != NULL)
	      delete fiber->environment;

	    // delete the cached fiber.
	    delete fiber;
	  }
      }

      //
      // finally, delete all the fibers waiting for something.
      //
      {
        Fiber::F::Scoutor	scoutor;

	// then go through the blocked fibers container.
	for (scoutor = Fiber::Fibers.begin();
	     scoutor != Fiber::Fibers.end();
	     scoutor++)
	  {
	    Fiber*	fiber = *scoutor;

	    // delete the fiber's environment.
	    if (fiber->environment != NULL)
	      delete fiber->environment;

	    // delete the waiting fiber.
	    delete fiber;
	  }
      }

      //
      // delete also the program.
      //
      {
        // delete the program's environment.
        if (Fiber::Program->environment != NULL)
          delete Fiber::Program->environment;

	// delete the program fiber.
	delete Fiber::Program;
      }

      leave();
    }

    ///
    /// this method puts the current fiber to sleep for the given duration.
    ///
    /// in order to wake up the fiber, a timer is set up.
    ///
    /// note that the duration is expressed in milliseconds.
    ///
    Status		Fiber::Sleep(const Natural32		duration)
    {
      enter();

      // allocate the timer.
      Fiber::Current->timer = new Timer;

      // create the timer.
      if (Fiber::Current->timer->Create(Timer::ModeSingle) == StatusError)
        escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (Fiber::Current->timer->signal.timeout.Subscribe(
            Callback<>::Infer(&Fiber::Timeout, Fiber::Current)) == StatusError)
        escape("unable to subscribe to the signal");

      // set up the timer.
      if (Fiber::Current->timer->Start(duration) == StatusError)
        escape("unable to start the timer");

      // wait for the resource represented by the timer's address.
      if (Fiber::Wait(Fiber::Current->timer) == StatusError)
        escape("unable to wait for the resource");

      leave();
    }

    ///
    /// this method registers a callback to be trigger should a fiber
    /// need to be saved or restored, depending on the given phase.
    ///
    Status		Fiber::Register(const
                                          Callback<
                                            Status,
                                            Parameters<Phase, Fiber*>
                                            >			c)
    {
      Callback< Status,
                Parameters<Phase, Fiber*> >*	callback;

      enter();

      // clone the callback.
      callback = new Callback< Status,
                               Parameters<Phase, Fiber*> >(c);

      // store in the container.
      Fiber::Phases.push_back(callback);

      leave();
    }

    ///
    /// this method triggers the callbacks associated with the phase.
    ///
    Status		Fiber::Trigger(Phase			phase)
    {
      Fiber::P::Scoutor	scoutor;

      enter();

      // go through the appropriate container.
      for (scoutor = Fiber::Phases.begin();
           scoutor != Fiber::Phases.end();
           scoutor++)
        {
          // trigger the callback, passing the current fiber.
          if ((*scoutor)->Call(phase, Fiber::Current) == StatusError)
            escape("an error occured in the callback");
        }

      leave();
    }

    ///
    /// this method takes the first awaken fiber and schedules it.
    ///
    Status		Fiber::Schedule()
    {
      Fiber::F::Iterator	iterator;

      enter();

      if (Fiber::IsScheduling)
        escape("don't call Fiber::Schedule() if Fiber::IsScheduling is true.");
      Fiber::IsScheduling = true;

      // if there is not fibers, return.
      if (Fiber::Fibers.empty() == true)
        leave();

      // iterate over the container.
      for (iterator = Fiber::Fibers.begin();
           iterator != Fiber::Fibers.end();
           )
        {
          Fiber*		fiber = *iterator;

	  // if this fiber needs scheduling.
	  if (fiber->state == Fiber::StateAwaken)
	    {
	      // remove the fiber from the container.
	      if (Fiber::Remove(fiber) == StatusError)
                {
                  Fiber::IsScheduling = false;
                  escape("unable to remove the fiber");
                }

              // save the environment.
              if (Fiber::Trigger(PhaseSave) == StatusError)
                escape("unable to save the environment");

	      // set the current fiber as suspended.
	      Fiber::Current->state = Fiber::StateSuspended;

	      // waiting for another fiber i.e the fiber to be scheduled.
	      Fiber::Current->type = Fiber::TypeFiber;

	      // switch the current fiber.
	      Fiber::Current = fiber;

	      // set as active.
	      Fiber::Current->state = Fiber::StateActive;

	      // restore the environment.
	      if (Fiber::Trigger(PhaseRestore) == StatusError)
                {
                  Fiber::IsScheduling = false;
                  escape("unable to restore the environment");
                }

              // set the context of the suspended fiber.
	      if (::swapcontext(&Fiber::Program->context,
				&Fiber::Current->context) == -1)
                {
                  Fiber::IsScheduling = false;
                  escape("unable to swapcontext");
                }

              // check if the current fiber state
              Fiber::CheckCurrentFiber();

              iterator = Fiber::Fibers.begin();
	    }
          else
            iterator++;
	}

      Fiber::IsScheduling = false;
      leave();
    }

    ///
    /// this method allocates a new fiber but may also take an already
    /// created and unused fiber from the cache.
    ///
    Status		Fiber::New(Fiber*&			fiber)
    {
      enter();

      // check the cache.
      if (Fiber::Cache.empty() == false)
        {
          // return the cached fiber.
          fiber = Fiber::Cache.front();

	  // remove the picked cached fiber.
	  Fiber::Cache.pop_front();
	}
      else
        {
          // otherwise, allocate a new fiber.
          fiber = new Fiber;

	  // create the fiber.
	  if (fiber->Create() == StatusError)
	    escape("unable to create the fiber");
	}

      // allocate an environment.
      fiber->environment = new Environment;
      memset(&fiber->context, 0, sizeof (fiber->context));

      leave();
    }

    ///
    /// this method deletes a fiber.
    ///
    Status		Fiber::Delete(Fiber*			fiber)
    {
      enter();

      // delete the environment.
      delete fiber->environment;

      // do not delete the fiber and store in the cache if the cache
      // is empty.
      if (Fiber::Cache.size() == Fiber::Capacity)
        {
          // reset the fiber's attributes but keep the stack.
          fiber->link = NULL;
          fiber->state = Fiber::StateUnknown;
          fiber->type = Fiber::TypeNone;
          fiber->environment = NULL;
          fiber->data = NULL;

	  // store it in the cache.
	  Fiber::Cache.push_front(fiber);
	}
      else
        {
          // otherwise, delete the fiber.
          delete fiber;
        }

      leave();
    }

    ///
    /// this method adds a fiber waiting for an event to the container.
    ///
    Status		Fiber::Add(Fiber*			fiber)
    {
      enter();

      // ignore the program fiber which is special as it is
      // used as the root fiber.
      if (fiber == Fiber::Program)
        leave();

      // push the fiber.
      Fiber::Fibers.push_front(fiber);

      leave();
    }

    ///
    /// this method removes a fiber from the container.
    ///
    Status		Fiber::Remove(Fiber*			fiber)
    {
      Fiber::F::Iterator	iterator;

      enter();

      // ignore the program fiber which is special as it is
      // used as the root fiber.
      if (fiber == Fiber::Program)
        leave();

      // iterate over the container.
      for (iterator = Fiber::Fibers.begin();
           iterator != Fiber::Fibers.end();
           iterator++)
        {
          Fiber*	f = *iterator;

	  if (fiber == f)
	    {
	      // remove the fiber.
	      Fiber::Fibers.erase(iterator);

	      // return since a fiber cannot be waiting for another
	      // event or resource hence cannot be located twice in
	      // the containers.
	      leave();
	    }
	}

      escape("unable to locate the fiber to remove");
    }

    ///
    /// this method locates the first fiber from the container waiting on
    /// the given event.
    ///
    Status		Fiber::Locate(const Event&		event,
                                      F::Iterator&		iterator)
    {
      enter();

      // iterator over the container.
      for (iterator = Fiber::Fibers.begin();
           iterator != Fiber::Fibers.end();
           iterator++)
        {
          Fiber*	fiber = *iterator;

	  // check if this fiber is waiting for the given event.
	  if ((fiber->state == Fiber::StateSuspended) &&
	      (fiber->type == Fiber::TypeEvent) &&
	      (*fiber->event == event))
	    true();
	}

      false();
    }

    ///
    /// this method locates the first fiber from the container waiting on
    /// the given resource.
    ///
    Status		Fiber::Locate(const Resource*		resource,
                                      F::Iterator&		iterator)
    {
      enter();

      // iterator over the container.
      for (iterator = Fiber::Fibers.begin();
           iterator != Fiber::Fibers.end();
           iterator++)
        {
          Fiber*	fiber = *iterator;

	  // check if this fiber is waiting for the given resource.
	  if ((fiber->state == Fiber::StateSuspended) &&
	      (fiber->type == Fiber::TypeResource) &&
	      (fiber->resource == resource))
	    true();
	}

      false();
    }

    ///
    /// this method dumps the fiber system state.
    ///
    Status		Fiber::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Fiber] " << std::endl;

      // dump the fibers.
      {
        Fiber::F::Scoutor	scoutor;

        std::cout << alignment << Dumpable::Shift << "[Fibers]" << std::endl;

	// dump every fiber waiting for an event/resource.
	for (scoutor = Fiber::Fibers.begin();
	     scoutor != Fiber::Fibers.end();
	     scoutor++)
	  {
	    Fiber*	fiber = *scoutor;

	    // dump the fiber.
	    if (fiber->Dump(margin + 4) == StatusError)
	      escape("unable to dump the fiber");
	  }
      }

      // dump the program.
      std::cout << alignment << Dumpable::Shift
                << "[Program]" << std::endl;

      if (Fiber::Program->Dump(margin + 4) == StatusError)
        escape("unable to dump the program fiber");

      // dump the current fiber.
      std::cout << alignment << Dumpable::Shift << "[Current]"
                << std::endl;

      if (Fiber::Current->Dump(margin + 4) == StatusError)
        escape("unable to dump the current fiber");

      // dump the cache.
      {
        Fiber::F::Scoutor	scoutor;

        std::cout << alignment << Dumpable::Shift << "[Cache]" << std::endl;

	// dump every fiber of the cache.
	for (scoutor = Fiber::Cache.begin();
	     scoutor != Fiber::Cache.end();
	     scoutor++)
	  {
	    Fiber*	fiber = *scoutor;

	    std::cout << alignment << Dumpable::Shift << Dumpable::Shift
		      << std::hex << fiber << std::endl;
	  }
      }

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Fiber::Fiber():
      link(NULL),
      frame(NULL),
      state(Fiber::StateUnknown),
      type(Fiber::TypeNone),
      event(NULL),
      environment(NULL),
      data(NULL),
      timer(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Fiber::~Fiber()
    {
      // release the frame.
      if (this->frame != NULL)
        delete this->frame;

      // release the timer.
      if (this->timer != NULL)
        delete this->timer;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes a new fiber.
    ///
    Status		Fiber::Create(const Natural32		size)
    {
      enter();

      // allocate the frame.
      this->frame = new Frame;

      // create the frame.
      if (this->frame->Create(size) == StatusError)
        escape("unable to create the frame");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered whenever the fiber needs to be woken up.
    ///
    Status		Fiber::Timeout()
    {
      enter();

      // awaken the fiber.
      if (Fiber::Awaken(this->timer) != StatusTrue)
        escape("unable to awaken the fiber");

      // don't delete the timer because it is in use
      this->timer->Stop();
      bury(this->timer);

      // reset the pointer.
      this->timer = NULL;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the fiber.
    ///
    Status		Fiber::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Fiber] " << std::hex << this << std::endl;

      // dump the link
      std::cout << alignment << Dumpable::Shift << "[Link] "
                << std::hex << this->link << std::endl;

      // dump the frame.
      if (this->frame != NULL)
        {
          if (this->frame->Dump(margin + 2) == StatusError)
            escape("unable to dump the frame");
        }
      else
        std::cout << alignment << Dumpable::Shift
                  << "[Frame] null" << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[State] "
                << this->state << std::endl;

      // dump the type.
      std::cout << alignment << Dumpable::Shift << "[Type] "
                << this->type << std::endl;

      switch (this->type)
        {
        case Fiber::TypeEvent:
          {
            // dump the event.
            if (this->event->Dump(margin + 4) == StatusError)
              escape("unable to dump the event");

	    break;
	  }
	case Fiber::TypeResource:
	  {
	    // dump the resource.
	    std::cout << alignment << Dumpable::Shift << "[Resource] "
		      << std::hex << this->resource << std::endl;

	    break;
	  }
	case Fiber::TypeFiber:
	case Fiber::TypeNone:
	  {
	    break;
	  }
	}

      // dump the environment.
      if (this->environment->Dump(margin + 2) == StatusError)
        escape("unable to dump the environment");

      // dump the data value.
      std::cout << alignment << Dumpable::Shift << "[Data] "
                << std::hex << this->data << std::endl;

      // dump the timer.
      if (this->timer != NULL)
        {
          if (this->timer->Dump(margin + 2) == StatusError)
            escape("unable to dump the timer");
        }
      else
        {
          // dump none.
          std::cout << alignment << Dumpable::Shift << "[Timer] "
                    << none << std::endl;
        }

      leave();
    }

    Status              Fiber::CheckCurrentFiber()
    {
      enter();

      if (!Fiber::Current)
        escape("you should not call %s with null Fiber::Current",
               __PRETTY_FUNCTION__);

      //
      // at this point, we just came back from a fiber.
      //

      // perform an action depending on the state of the fiber.
      switch (Fiber::Current->state)
        {
        case Fiber::StateCompleted:
          {
            // clean the environment.
            if (Fiber::Trigger(PhaseClean) == StatusError)
              escape("unable to initialize the environment");

            // if the fiber has completed, delete it.
            if (Fiber::Delete(Fiber::Current) == StatusError)
              escape("unable to delete the fiber");

            break;
          }
        case Fiber::StateSuspended:
          {
            //
            // do not delete this fiber as it will be resumed later.
            //
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

      // set the current fiber as being the selected fiber.
      Fiber::Current = Fiber::Program;

      // set the fiber as active.
      Fiber::Current->state = Fiber::StateActive;

      // restore the environment.
      if (Fiber::Trigger(PhaseRestore) == StatusError)
        escape("unable to restore the environment");

      if (!Fiber::IsScheduling)
        Schedule();

      leave();
    }

  }
}
