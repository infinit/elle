//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 25 12:48:50 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Hurdle.hh>
#include <elle/concurrency/Fiber.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- zone ------------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Hurdle::Zone::Zone(Hurdle&                                  hurdle,
                       const Mode                               mode):
      hurdle(hurdle),
      mode(mode),

      section(L(C(&Hurdle::Lock, &this->hurdle), this->mode),
              U(C(&Hurdle::Unlock, &this->hurdle), this->mode))
    {
    }

    ///
    /// this method locks the hurdle according to the zone's mode.
    ///
    Void                Hurdle::Zone::Lock()
    {
      // call the section's Enter() method.
      this->section.Enter();
    }

    ///
    /// this method unlocks the hurdle according to the zone's mode.
    ///
    Void                Hurdle::Zone::Unlock()
    {
      // call the section's Leave() method.
      this->section.Leave();
    }

    ///
    /// this method dumps the hurdle.
    ///
    Status              Hurdle::Zone::Dump(const Natural32      margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Zone]" << std::endl;

      // dump the hurdle.
      if (this->hurdle.Dump(margin + 2) == StatusError)
        escape("unable to dump the hurdle");

      // dump the section.
      if (this->section.Dump(margin + 2) == StatusError)
        escape("unable to dump the section");

      return elle::StatusOk;
    }

//
// ---------- hurdle ----------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Hurdle::Hurdle():
      locked(false),
      readers(0)
    {
    }

    ///
    /// this method locks the hurdle, if possible, or blocks the fiber
    /// until the hurdle gets unlocked.
    ///
    /// note that the mode indicates the type of access.
    ///
    Void                Hurdle::Lock(const Mode                 mode)
    {
      // has the lock been acquired, wait for it to get released.
      while (this->locked == true)
        {
          //
          // in this case, no matter the mode, the current fiber
          // must be blocked until the hurdle gets released.
          //

          // wait for the hurdle.
          if (Fiber::Wait(*this) == StatusError)
            yield(_(), "an error occured while waiting on the resource");
        }

      // then, depending on the mode.
      switch (mode)
        {
        case ModeRead:
          {
            // increase the number of readers.
            this->readers++;

            //
            // and let the caller access the protected object.
            //

            break;
          }
        case ModeWrite:
          {
            // first, set the hurdle as being locked so that another
            // writer does not acquire it.
            this->locked = true;

            // wait for readers to leave the critical section.
            while (this->readers != 0)
              {
                //
                // the current fiber must be blocked until the number
                // of readers comes down to zero.
                //

                // wait for the hurdle.
                if (Fiber::Wait(*this) == StatusError)
                  yield(_(), "an error occured while waiting on the resource");
              }

            //
            // at this point, the current fiber has acquired the lock
            // and there is not any reader left. the writer can therefore
            // proceed.
            //

            break;
          }
        default:
          {
            yield(_(), "unknown mode '%u'",
                  mode);
          }
        }
    }

    ///
    /// this method unlocks the hurdle according to the access mode.
    ///
    Void                Hurdle::Unlock(const Mode               mode)
    {
      // then, depending on the mode.
      switch (mode)
        {
        case ModeRead:
          {
            // decrease the number of readers.
            this->readers--;

            break;
          }
        case ModeWrite:
          {
            // reset the lock as being available;
            this->locked = false;

            break;
          }
        default:
          {
            yield(_(), "unknown mode '%u'",
                  mode);
          }
        }

      // and finally, awaken the fibers potentially blocked on the
      // hurdle.
      if (Fiber::Awaken(*this) == StatusError)
        yield(_(), "unable to awaken the fibers");
    }

    ///
    /// this method returns true if the given access would be authorised
    /// without blocking.
    ///
    Status              Hurdle::Try(const Mode                  mode)
    {
      // has the lock been acquired, return false.
      if (this->locked == true)
        return elle::StatusFalse;

      // then, depending on the mode.
      switch (mode)
        {
        case ModeRead:
          {
            //
            // return true since the lock has not been acquired and
            // the access is requested in read.
            //

            break;
          }
        case ModeWrite:
          {
            // check the number of readers and return false if some remain.
            if (this->readers != 0)
              return elle::StatusFalse;

            //
            // otherwise, return true.
            //

            break;
          }
        default:
          {
            escape("unknown mode '%u'",
                  mode);
          }
        }

      return elle::StatusTrue;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Hurdle, _());

    ///
    /// this method dumps the hurdle.
    ///
    Status              Hurdle::Dump(const Natural32            margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Hurdle]" << std::endl;

      // dump the locked boolean.
      std::cout << alignment << Dumpable::Shift
                << "[Locked] " << this->locked << std::endl;

      // dump the number of readers.
      std::cout << alignment << Dumpable::Shift
                << "[Readers] " << std::dec << this->readers << std::endl;

      return elle::StatusOk;
    }

  }
}
