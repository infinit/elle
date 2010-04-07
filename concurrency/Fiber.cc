//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Fiber.cc
//
// created       julien quintard   [mon mar 22 02:22:43 2010]
// updated       julien quintard   [tue mar 30 00:23:01 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Fiber.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace concurrency
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this value defines fibers' stack size, in bytes: 32KB
    ///
    const Natural32		Fiber::Size = 32768;

    ///
    /// this value defines the fibers cache capacity.
    ///
    const Natural32		Fiber::Capacity = 3;

    ///
    /// this variable holds the fibers waiting or that have been waiting
    /// for an event/resource.
    ///
    Fiber::F::Container		Fiber::Fibers;

    ///
    /// this fiber corresponds to the application just when entering
    /// the processing of an event.
    ///
    Fiber*			Fiber::Application = NULL;

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
    Void*			Fiber::Trash = NULL;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the fiber system.
    ///
    Status		Fiber::Initialize()
    {
      enter();

      // allocate the application fiber but do not create it since
      // this application has no need for a stack.
      Fiber::Application = new Fiber;

      // set the application as being running.
      Fiber::Application->state = Fiber::StateActive;

      // set the current fiber as being the application.
      Fiber::Current = Fiber::Application;

      leave();
    }

    ///
    /// this method cleans the fiber system.
    ///
    Status		Fiber::Clean()
    {
      enter();

      // XXX clean toutes les fibers de events/resources

      leave();
    }

    ///
    /// this method takes the first awaken fiber and schedules it.
    ///
    Status		Fiber::Schedule()
    {
      Fiber::F::Iterator	iterator;

      enter();

      // if there is not fibers, return.
      if (Fiber::Fibers.empty() == true)
	leave();

      //printf("[XXX 0x%x] Fiber::Schedule()\n", Fiber::Current);
      //Fiber::Show();

      // iterate over the container.
      for (iterator = Fiber::Fibers.begin();
	   iterator != Fiber::Fibers.end();
	   iterator++)
	{
	  Fiber*		fiber = *iterator;

	  // if this fiber needs scheduling.
	  if (fiber->state == Fiber::StateAwaken)
	    {
	      //printf("[XXX 0x%x] Fiber::Schedule() :: 0x%x\n",
	      //Fiber::Current, fiber);

	      // remove the fiber from the container.
	      if (Fiber::Remove(fiber) == StatusError)
		escape("unable to remove the fiber");

	      // set as active.
	      fiber->state = Fiber::StateActive;

	      // set the current fiber as suspended.
	      Fiber::Current->state = Fiber::StateSuspended;

	      // waiting for another fiber i.e the fiber to be scheduled.
	      Fiber::Current->type = Fiber::TypeFiber;

	      // switch the current fiber.
	      Fiber::Current = fiber;

	      // set the context of the suspended fiber.
	      if (::setcontext(&Fiber::Current->context) == -1)
		escape("unable to set the context");

	      //
	      // since the setcontext() function never returns we should
	      // never reach that point.
	      ///
	      release();
	      fail("this code should never have been reached");
	    }
	}

      leave();
    }

    ///
    /// this method allocates a new fiber but may also take an already
    /// created and unused fiber from the cache.
    ///
    Status		Fiber::New(Fiber*&			fiber)
    {
      enter();

      //printf("[XXX 0x%x] Fiber::New()\n",
      //Fiber::Current);

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

      leave();
    }

    ///
    /// this method deletes a fiber.
    ///
    Status		Fiber::Delete(Fiber*			fiber)
    {
      enter();

      //printf("[XXX 0x%x] Fiber::Delete(0x%x)\n",
      //Fiber::Current, fiber);

      // do not delete the fiber and store in the cache if the cache
      // is empty.
      if (Fiber::Cache.size() == Fiber::Capacity)
	{
      	  // reset the fiber's attributes but keep the stack.
	  fiber->link = NULL;
	  fiber->state = Fiber::StateUnknown;
	  fiber->type = Fiber::TypeNone;
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

      //printf("[XXX 0x%x] Fiber::Add(0x%x)\n",
      //Fiber::Current, fiber);

      // ignore the application fiber which is special as it is
      // used as the root fiber.
      if (fiber == Fiber::Application)
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

      //printf("[XXX 0x%x] Fiber::Remove(0x%x)\n",
      //Fiber::Current, fiber);

      // ignore the application fiber which is special as it is
      // used as the root fiber.
      if (fiber == Fiber::Application)
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

      // dump the application.
      std::cout << alignment << Dumpable::Shift
		<< "[Application]" << std::endl;

      if (Fiber::Application->Dump(margin + 4) == StatusError)
	escape("unable to dump the application fiber");

      // dump the current fiber.
      std::cout << alignment << Dumpable::Shift << "[Current]"
		<< std::endl;

      if (Fiber::Current->Dump(margin + 4) == StatusError)
	escape("unable to dump the current fiber");

      // dump the cache.
      {
	Fiber::F::Scoutor	scoutor;

	std::cout << alignment << Dumpable::Shift << "[Cache] " << std::endl;

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
      data(NULL)
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
	}

      // dump the data value.
      std::cout << alignment << Dumpable::Shift << "[Data] "
		<< std::hex << this->data << std::endl;

      leave();
    }

  }
}
