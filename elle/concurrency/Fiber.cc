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
// updated       julien quintard   [thu mar 25 22:50:15 2010]
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
    /// this variable holds the suspended or awaken fibers.
    ///
    Fiber::Container		Fiber::Fibers;

    ///
    /// this fiber corresponds to the application just when entering
    /// the processing of an event.
    ///
    Fiber			Fiber::Application;

    ///
    /// this variable holds the frame that will be used for processing event.
    ///
    /// indeed, without such a specific frame, the system would allocate a
    /// stack whenever an event is to be processed.
    ///
    /// since most events do not block, there is no need for fibers. it would
    /// therefore greatly impact the system should a stack be allocated
    /// for every event.
    ///
    Fiber*			Fiber::Handler = NULL;

    ///
    /// this variable points to the fiber currently in use.
    ///
    Fiber*			Fiber::Current = NULL;

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
    /// note that the application and handler fibers are never
    /// stored in the container.
    ///
    Status		Fiber::Initialize()
    {
      enter();

      // allocate the event fiber;
      Fiber::Handler = new Fiber;

      // create the handler's fiber.
      if (Fiber::Handler->Create() == StatusError)
	escape("unable to create the handler fiber");

      // set the current fiber as being the application.
      Fiber::Current = &Fiber::Application;

      leave();
    }

    ///
    /// this method cleans the fiber system.
    ///
    Status		Fiber::Clean()
    {
      enter();

      // XXX clean toutes les fibers

      leave();
    }

    ///
    /// this method takes the first awaken fiber and schedules it.
    ///
    Status		Fiber::Schedule()
    {
      Fiber::Iterator	iterator;

      enter();

      //printf("[XXX 0x%x] Fiber::Schedule()\n", Fiber::Current);

      // if there is not fibers, return.
      if (Fiber::Fibers.empty() == true)
	leave();

      // iterate over the container elements.
      for (iterator = Fiber::Fibers.begin();
	   iterator != Fiber::Fibers.end();
	   iterator++)
	{
	  Fiber*	fiber = *iterator;

	  // if this fiber needs scheduling.
	  if (fiber->state == Fiber::StateAwaken)
	    {
	      //printf("[XXX 0x%x] Fiber::Schedule() :: 0x%x\n",
	      //Fiber::Current, fiber);

	      // set as active
	      fiber->state = Fiber::StateActive;

	      // set the current fiber.
	      Fiber::Current = fiber;

	      // set the context of the suspended fiber.
	      if (::setcontext(&fiber->context) == -1)
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
    /// this method adds a fiber to the container.
    ///
    Status		Fiber::Add(Fiber*			fiber)
    {
      enter();

      // push the fiber.
      Fiber::Fibers.push_front(fiber);

      leave();
    }

    ///
    /// this method tries to locate a fiber.
    ///
    Status		Fiber::Locate(const Fiber*		fiber,
				      Fiber::Iterator&		iterator)
    {
      enter();

      // iterate over the container elements.
      for (iterator = Fiber::Fibers.begin();
	   iterator != Fiber::Fibers.end();
	   iterator++)
	if (fiber == *iterator)
	  true();

      false();
    }

    ///
    /// this method tries to locate a fiber waiting for the given event.
    ///
    Status		Fiber::Locate(const Event&		event,
				      Fiber::Iterator&		iterator)
    {
      enter();

      // iterator over the container.
      for (iterator = Fiber::Fibers.begin();
	   iterator != Fiber::Fibers.end();
	   iterator++)
	{
	  Fiber*	fiber = *iterator;

	  // check if this fiber is waiting for the given event.
	  if (event == fiber->event)
	    true();
	}

      false();
    }

    ///
    /// this method removes a fiber from the container.
    ///
    Status		Fiber::Remove(Fiber*			fiber)
    {
      Fiber::Iterator	iterator;

      //printf("[XXX 0x%x] Fiber::Remove(0x%x)\n",
      //Fiber::Current, fiber);

      enter();

      // iterate over the container elements.
      for (iterator = Fiber::Fibers.begin();
	   iterator != Fiber::Fibers.end();
	   iterator++)
	if (fiber == *iterator)
	  {
	    Fiber::Fibers.erase(iterator);

	    leave();
	  }

      escape("unable to locate the fiber to remove");
    }

    ///
    /// this method dumps the fiber system state.
    ///
    Status		Fiber::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Fiber::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Fiber] " << std::endl;

      // dump all the fibers.
      std::cout << alignment << Dumpable::Shift << "[Fibers] " << std::endl;

      for (scoutor = Fiber::Fibers.begin();
	   scoutor != Fiber::Fibers.end();
	   scoutor++)
	if ((*scoutor)->Dump(margin + 4) == StatusError)
	  escape("unable to dump the fiber");

      // dump the application.
      std::cout << alignment << Dumpable::Shift
		<< "[Application]" << std::endl;

      if (Fiber::Application.Dump(margin + 4) == StatusError)
	escape("unable to dump the application fiber");

      // dump the handler.
      std::cout << alignment << Dumpable::Shift
		<< "[Handler]" << std::endl;

      if (Fiber::Handler->Dump(margin + 4) == StatusError)
	escape("unable to dump the handler fiber");

      // dump the current fiber pointer.
      std::cout << alignment << Dumpable::Shift << "[Current] "
		<< std::hex << Fiber::Current << std::endl;

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Fiber::Fiber():
      frame(NULL),
      state(Fiber::StateUnknown),
      status(StatusUnknown),
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

      // dump the status.
      std::cout << alignment << Dumpable::Shift << "[Status] "
		<< this->status << std::endl;

      // dump the event.
      if (this->event.Dump(margin + 2) == StatusError)
	escape("unable to dump the event");

      leave();
    }

  }
}
