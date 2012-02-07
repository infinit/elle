//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue mar 23 14:55:13 2010]
//

#ifndef ELLE_CONCURRENCY_FIBER_HXX
#define ELLE_CONCURRENCY_FIBER_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
    Void                Fiber::Launch(Closure<Status, T...>*    closure)
    {
      //
      // trigger the closure and, should there are errors, display them.
      //

      if (closure->Call() == StatusError)
        yield(_(), "an error occured in the fiber");

      //
      // set the fiber state i.e completed.
      //

      Fiber::Current->state = Fiber::StateCompleted;

      //
      // finally, prepare to schedule the parent fiber by
      // removing the parent from the waiting list and set
      // its state as awken.

      if (Fiber::Remove(Fiber::Current->link) == StatusError)
        yield(_(), "unable to remove the fiber");

      Fiber::Current->link->state = Fiber::StateAwaken;

      //
      // finally, switch back to the main thread.
      //

      if (::epth_switch(Fiber::Current->context,
                        Fiber::Program->context) == StatusError)
        yield(_(), "unable to switch back to the program thread");
    }

    ///
    /// this method spawns a fiber.
    ///
    template <typename... T>
    Status              Fiber::Spawn(Closure<Status, T...>&     closure)
    {
      //
      // if we are already in a fiber, don't create another one: simply
      // trigger the closure.
      //

      if (Fiber::Current != Fiber::Program)
        {
          if (closure.Call() == StatusError)
            escape("an error occured in the fiber");

          return elle::StatusOk;
        }

      //
      // set the current fiber as suspended and waiting for a child
      // fiber.
      //

      Fiber::Current->state = Fiber::StateSuspended;
      Fiber::Current->type = Fiber::TypeFiber;

      //
      // save the current fiber's environment i.e report, session etc.
      //

      if (Fiber::Trigger(PhaseSave) == StatusError)
        escape("unable to save the environment");

      //
      // add the current fiber to the waiting list.
      //

      if (Fiber::Add(Fiber::Current) == StatusError)
        escape("unable to add the fiber to the container");

      //
      // declare a launch function pointer in order to bypass the type
      // checking system through casts.
      //

      Void              (*launch)(Closure<Status, T...>*) = &Fiber::Launch;
      Fiber*            fiber;

      //
      // allocate a new fiber and initialize it.
      //

      if (Fiber::New(fiber) == StatusError)
        escape("unable to allocate a new fiber");

      fiber->link = Fiber::Current;
      fiber->state = Fiber::StateActive;

      //
      // spawn the actual thread.
      //

      if (::epth_spawn(reinterpret_cast<void* (*)(void*)>(launch),
                       &closure,
                       fiber->context) == StatusError)
        escape("unable to spawn the thread");

      //
      // set the new fiber as the current one.
      //

      Fiber::Current = fiber;

      //
      // initialize the new fiber's environment.
      //

      if (Fiber::Trigger(PhaseInitialize) == StatusError)
        escape("unable to initialize the environment");

      //
      // finally, switch on the new thread.
      //

      if (::epth_switch(Fiber::Program->context,
                        Fiber::Current->context) == StatusError)
        escape("unable to switch to the new thread");

      // XXX Why not return the _CheckCurrentFiber() value ??
      Fiber::_CheckCurrentFiber();

      return elle::StatusOk;
    }


    ///
    /// this method takes the current fiber and sets the event it is
    /// expected to continue.
    ///

    template <typename T>
    Status Fiber::Wait(const Event& event, std::shared_ptr<T>& data)
    {
      if (Fiber::Wait(event) == StatusError)
        return StatusError;

      // retrieve the data.
      // XXX Meta -> User type : Y U NO DYNAMIC_CAST ?
      data = std::static_pointer_cast<T>(Fiber::Current->data);
      // XXX Reset fiber data ?

      return elle::StatusOk;
    }

    ///
    /// this method takes the current fiber and sets the resource it is
    /// expected to continue.
    ///
    /// note that resources are expected to be objects located in main
    /// memory i.e memory addresses. since memory addresses are unique,
    /// this simple scheme prevents conflicts.
    ///

    template <typename T>
    Status Fiber::Wait(Resource const& resource, std::shared_ptr<T>& data)
    {
      if (Fiber::Wait(resource) == StatusError)
        return StatusError;

      // retrieve the data.
      // XXX Meta -> User type : Y U NO DYNAMIC_CAST ?
      data = std::static_pointer_cast<T>(Fiber::Current->data);
      // XXX Reset fiber data ?

      return elle::StatusOk;
    }

    ///
    /// this method wakes up the fiber waiting for the given event.
    ///
    template <typename DataType>
    Status Fiber::Awaken(const Event& event, std::shared_ptr<DataType> data)
    {
      Fiber::W::Iterator        iterator;
      Boolean                   awaken;

      // check if there are blocked fibers.
      if (Fiber::Waiting.empty() == true)
        return elle::StatusFalse;

      // set the boolean to false meaning that no fiber has been woken up.
      awaken = false;

      // locate, awaken and remove fibers as long as found.
      while (Fiber::Locate(event, iterator) == true)
        {
          Fiber*        fiber = *iterator;

          // set the boolean to true.
          awaken = true;

          // set the data.
          fiber->data = data;

          // set the state as awaken.
          fiber->state = Fiber::StateAwaken;

          // reset the type.
          fiber->type = Fiber::TypeNone;

          // delete and reset the event.
          delete fiber->event;
          fiber->event = NULL;
        }

      // return true if at least one fiber has been awaken.
      if (awaken == true)
        return elle::StatusTrue;

      return elle::StatusFalse;
    }

    ///
    /// this method wakes up the fibers waiting for the given resource.
    ///
    template <typename DataType>
    Status Fiber::Awaken(const Resource& resource, std::shared_ptr<DataType> data)
    {
      Fiber::W::Iterator iterator;
      Boolean            awaken;

      // check if there are blocked fibers.
      if (Fiber::Waiting.empty() == true)
        return elle::StatusFalse;

      // set the boolean to false meaning that no fiber has been woken up.
      awaken = false;

      // locate, awaken and remove fibers as long as found.
      while (Fiber::Locate(&resource, iterator) == true)
        {
          Fiber*        fiber = *iterator;

          // set the boolean to true.
          awaken = true;

          // set the data.
          fiber->data = data;

          // set the state as awaken.
          fiber->state = Fiber::StateAwaken;

          // reset the type.
          fiber->type = Fiber::TypeNone;

          // reset the resource.
          fiber->resource = NULL;
        }

      // return true if at least one fiber has been awaken.
      if (awaken == true)
        return elle::StatusTrue;

      return elle::StatusFalse;
    }

  }
}

#endif
