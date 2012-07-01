//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri oct 28 12:44:15 2011]
//

#ifndef ELLE_CONCURRENCY_SECTION_HXX
#define ELLE_CONCURRENCY_SECTION_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename... L,
              typename... U>
    Section< Parameters<L...>,
             Parameters<U...> >::Section(Closure<
                                           Void,
                                           Parameters<L...>,
                                           Parameters<>
                                           >                    lock,
                                         Closure<
                                           Void,
                                           Parameters<U...>,
                                           Parameters<>
                                           >                    unlock):
      state(StateUnlocked),
      lock(lock),
      unlock(unlock)
    {
    }

    ///
    /// reference-specific constructor.
    ///
    template <typename... L,
              typename... U>
    Section< Parameters<L...>,
             Parameters<U...> >::Section(Closure<
                                           Void,
                                           Parameters<L...>,
                                           Parameters<>
                                           >&                   lock,
                                         Closure<
                                           Void,
                                           Parameters<U...>,
                                           Parameters<>
                                           >&                   unlock):
      state(StateUnlocked),
      lock(lock),
      unlock(unlock)
    {
    }

    ///
    /// destructor.
    ///
    template <typename... L,
              typename... U>
    Section< Parameters<L...>,
             Parameters<U...> >::~Section()
    {
      // if the section has been locked...
      if (this->state == Section::StateLocked)
        {
          // release the lock by triggering the unlock closure.
          this->unlock.Call();
        }
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method is used for entering the critical section.
    ///
    template <typename... L,
              typename... U>
    Void                Section< Parameters<L...>,
                                 Parameters<U...> >::Enter()
    {
      // check the object's state.
      if (this->state != Section::StateUnlocked)
        yield(_(), "unable to lock an already locked section");

      // trigger the lock closure.
      this->lock.Call();

      // set the object as locked.
      this->state = Section::StateLocked;
    }

    ///
    /// this method is used for leaving the critical section, action
    /// which would trigger the unlocking of the associated lock.
    ///
    template <typename... L,
              typename... U>
    Void                Section< Parameters<L...>,
                                 Parameters<U...> >::Leave()
    {
      // check the object's state.
      if (this->state != Section::StateLocked)
        yield(_(), "unable to unlock an unlocked section");

      // trigger the unlock closure.
      this->unlock.Call();

      // set the object as unlocked.
      this->state = Section::StateUnlocked;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the section.
    ///
    template <typename... L,
              typename... U>
    Status
    Section< Parameters<L...>,
             Parameters<U...> >::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Section]" << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift
                << "[State] " << std::dec << this->state << std::endl;

      // dump the lock closure.
      if (this->lock.Dump(margin + 2) == Status::Error)
        escape("unable to dump the closure");

      // dump the unlock closure.
      if (this->unlock.Dump(margin + 2) == Status::Error)
        escape("unable to dump the closure");

      return Status::Ok;
    }

  }
}

#endif
